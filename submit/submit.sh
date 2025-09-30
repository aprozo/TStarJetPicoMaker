#!/bin/bash
# -- baseFolder
baseFolder="$PWD"
# -- listOfFiles
listOfFiles="$PWD/$1"
# strip BASENAME from the path without the extension to get filelist_name range
filelist_name=$(basename "${listOfFiles}" .list)
# -- root macro
rootMacro="makeTStarJetPico.cxx"
# -- production Id
productionId=$(date +%F)
# -- set STAR software version
starVersion="pro"

#================================================================
# -- submission xml file
templateXml="template.xml"

jobFolder="${baseFolder}/submit/${productionId}/job_${filelist_name}"
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
[[ -e $listOfFiles ]] || {
    echo "$listOfFiles missing"
    exit 1
}
echo "ok"

# -- submit
generatedXml="generated_${filelist_name}.xml"

cat <<EOF >"${generatedXml}"
<?xml version="1.0" encoding="utf-8" ?>
<!DOCTYPE note [
<!ENTITY rootMacro "${rootMacro}">
<!ENTITY baseFolder "${baseFolder}">
<!ENTITY jobFolder "${jobFolder}">
<!ENTITY listOfFiles "${listOfFiles}">
<!ENTITY starVersion "${starVersion}">
<!ENTITY filelist_name "${filelist_name}">
]>

EOF
# -- add the rest of the xml file except the first line <?xml version="1.0" encoding="utf-8" ?>
tail -n +2 "${templateXml}" >>"${generatedXml}"

star-submit "${generatedXml}"
# star-submit-beta "${generatedXml}"
