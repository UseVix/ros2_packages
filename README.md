compression_and_decompression_statistic_saver - decompression_statistic_saver:
   parametry:
      topic: topic do zasubskrybowania
      compressiontype: metoda jaką zdekompresować otrzymaną wiadomość z chmurą
      bonusinfo: dodatkowe dane jakie dopisać do informacji o chmurze
Paczka subskrybuje topic z skompresowanymi chmurami a następnie zapisuje informacje związane z kompresją(wysokość, szerokość, czas dekompresji) do pliku tekstowego.
