# lists/ — production inputs

    test.list                   smoke-test input (macro default)
    pp200_mb_2012.list          -> data/pp200_production_2012_mudst.list
    pp200_towers_2012.list     -> data/pp200_production_2012_mudst.list
        (symlinks: template.xml resolves lists/<jobname>.list, so the job
         names live here while the single canonical list lives in data/)

    data/
      pp200_production_2012_mudst.list      canonical full Run-12 st_physics
                                            dataset, 76,560 MuDsts (incl. the
                                            adc stream) — drives BOTH data
                                            folders (configs JPHT and MB)
      pp200_production_2012_mudst_notInCatalog_20260708.list
                                            historical: files the catalog
                                            query missed (all contained in
                                            the canonical list)
      VPDMB_pp200_2012.list                 historical MB-round list
      single_run13048013.list               one-MuDst validation input

    emb2021/
      emb2021_20212001.list                 list-of-lists driving the 2021
                                            embedding production (7,813)
      list_of_lists_20212001_pt*.list       per-bin pieces (13 bins)
      runs/e21_<bin>_<run>.list             per-run MuDst lists
      exclude_pt2_3.txt                     6 short-minimc files excluded

    emb2023/
      emb2023_20235003.list                 list-of-lists driving the 2023
                                            embedding production (1,764)
      chunks/pt_hat*_NNN.list               per-chunk MuDst lists
      full/pt_hat_<bin>.list                per-bin full lists
