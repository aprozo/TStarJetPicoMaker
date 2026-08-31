#!/bin/bash
#
# Usage:
#   bash submit/submit.sh <list-or-tag> [submit-cmd]
#
# `<list-or-tag>` is either:
#   (a) a path to a .list file (filelist mode)  — its basename names the job folder
#       AND its contents drive the submission via the template's `<input filelist:.../>` line.
#   (b) a tag string, optionally with a .list extension that is empty/missing
#       (catalog mode) — only used to name the job folder; the actual input comes
#       from the template's `<input catalog:.../>` line.
#
# `[submit-cmd]` overrides the default `star-submit`. Useful values:
#   star-submit-beta     (newer scheduler; preferred for big productions)
#   ""                   (skip the actual submit; just generate the XML for review)
set -u

# -- baseFolder
baseFolder="$PWD"
# -- list/tag
listArg="${1:-}"
if [[ -z "$listArg" ]]; then
    echo "Usage: $0 <list-or-tag> [submit-cmd]"
    echo "  e.g.  $0 lists/single_test.list                       # filelist mode"
    echo "  e.g.  $0 pp200_p12id_full                             # catalog mode (template provides the catalog query)"
    exit 1
fi
# Resolve to absolute path if a real file is referenced; otherwise treat as a tag.
if [[ -e "$listArg" ]]; then
    listOfFiles="$(readlink -f "$listArg")"
elif [[ -e "$PWD/$listArg" ]]; then
    listOfFiles="$PWD/$listArg"
else
    listOfFiles="$PWD/$listArg"   # placeholder; not required to exist for catalog mode
fi
listBasename=$(basename "${listOfFiles}" .list)
# -- root macro
rootMacro="makeTStarJetPico.cxx"
# -- production Id
productionId=$(date +%F)
# -- set STAR software version
starVersion="pro"
# -- submission command (allow override; default is star-submit).
# `${2-…}` (no colon) defaults ONLY when $2 is unset. Passing an empty string ""
# explicitly means "skip the actual submit" — used for dry-runs.
submitCmd="${2-star-submit}"
# -- macro config (trigger set): ALL | JPHT | MB
macroConfig="${3:-ALL}"

#================================================================
# -- submission xml file
templateXml="template.xml"

jobFolder="${baseFolder}/submit/${productionId}/job_${listBasename}"
# -- job submission directory
mkdir -p "${jobFolder}"
cd "${jobFolder}"
# -- prepare folder
mkdir -p report err log list csh production

check=(
    "libs/libTStarJetPicoMaker.so" # shared library
    "libs/libTStarJetPico.so" # shared library
    "libs/libStRefMultCorr.so"     # shared library
    "macros/${rootMacro}" # run macro
    "submit/${templateXml}" # xml template
)

printf "Checking project … "
for item in "${check[@]}"; do
    path="${baseFolder}/${item}"
    [[ -e $path ]] || {
        echo "$item missing"
        exit 1
    }
    [[ -d $path || $item == *.xml ]] && ln -sf "$path"
done

# Decide whether the template uses a filelist or a catalog input.
# We grep the active (non-commented) <input> lines of template.xml.
inputLines=$(grep -E '^[[:space:]]*<input[[:space:]]' "${baseFolder}/submit/${templateXml}")
useFilelist=0
useCatalog=0
echo "$inputLines" | grep -q 'filelist:' && useFilelist=1
echo "$inputLines" | grep -q 'catalog:'  && useCatalog=1

if (( useFilelist && useCatalog )); then
    echo "WARNING: template has BOTH filelist: and catalog: <input> lines active." >&2
fi

if (( useFilelist )); then
    [[ -e $listOfFiles ]] || {
        echo "filelist mode: ${listOfFiles} missing"
        exit 1
    }
    echo "ok (filelist mode: ${listOfFiles})"
elif (( useCatalog )); then
    # No requirement on the list file's existence — listBasename is only used to
    # name the job folder.
    echo "ok (catalog mode: tag=${listBasename})"
else
    echo "ERROR: template has no active <input> line"
    exit 1
fi

# -- submit
generatedXml="generated_${listBasename}.xml"

cat <<EOF >"${generatedXml}"
<?xml version="1.0" encoding="utf-8" ?>
<!DOCTYPE note [
<!ENTITY rootMacro "${rootMacro}">
<!ENTITY baseFolder "${baseFolder}">
<!ENTITY jobFolder "${jobFolder}">
<!ENTITY listOfFiles "${listOfFiles}">
<!ENTITY starVersion "${starVersion}">
<!ENTITY listBasename "${listBasename}">
<!ENTITY macroConfig "${macroConfig}">
]>

EOF
# -- add the rest of the xml file except the first line <?xml version="1.0" encoding="utf-8" ?>
tail -n +2 "${templateXml}" >>"${generatedXml}"

if [[ -z "$submitCmd" ]]; then
    echo "(no submit-cmd given — skipping actual submission. XML is at ${jobFolder}/${generatedXml})"
else
    "${submitCmd}" "${generatedXml}"
fi
