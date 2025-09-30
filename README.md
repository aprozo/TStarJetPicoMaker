
# TStarJetPicoMaker
Used for production of special data format for jet analysis.
Macro [makeTStarJetPico.cxx](macros/makeTStarJetPico.cxx) accepted list of `MuDst` files from embedding and produces single file containing `JetTree` and `JetTreeMc`.

Current implementation is for dataset: "pp at 200 GeV, Run 12"



## Additional Libraries
These libraries require the `root4star` framework. Additionally,
[StRefMultCorr](https://github.com/GuannanXie/Run14AuAu200GeV_StRefMultCorr.git)
and [eventStructuredAu](https://github.com/kkauder/eventStructuredAu.git).
Versions that are compatible with the Maker are embedded in StRoot already. If
you want to update, they are kept as submodules in `external/`. You can
initialize the submodules via 
```bash
git submodule update --init --recursive
```

## Usage

After cloning the repository, you can use to compile the libraries via  
```bash
./macros/compile.sh
```
It will also `make` additional libraries in `external/`

to submit  a test job from SDCC:

```bash
./submit/submit.sh lists/test.list
```
