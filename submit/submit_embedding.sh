#!/bin/bash
# Submit embedding pico production. Pass list_of_lists.list as argument.
# Each entry in that file is a path to a per-pthat-bin .list of MuDsts.
# Usage: submit/submit_embedding.sh lists/list_of_lists.list

baseFolder="$PWD"
listOfFiles="$PWD/$1"
listBasename=$(basename "${listOfFiles}" .list)
rootMacro="makeTStarJetPicoEmbedding.cxx"
productionId=$(date +%F)
starVersion="pro"

templateXml="template_embedding.xml"

jobFolder="${baseFolder}/submit/${productionId}/job_${listBasename}"
mkdir -p "${jobFolder}"
cd "${jobFolder}"
mkdir -p report err log list csh production

check=(
    "libs/libTStarJetPicoMaker.so"
    "libs/libTStarJetPico.so"
    "libs/libStRefMultCorr.so"
    "macros/${rootMacro}"
    "submit/${templateXml}"
)

printf "Checking project … "
for item in "${check[@]}"; do
    path="${baseFolder}/${item}"
    [[ -e $path ]] || { echo "$item missing"; exit 1; }
    [[ -d $path || $item == *.xml ]] && ln -sf "$path"
done
[[ -e $listOfFiles ]] || { echo "$listOfFiles missing"; exit 1; }
echo "ok"

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
]>

EOF
tail -n +2 "${templateXml}" >>"${generatedXml}"

star-submit "${generatedXml}"
