cp OBJ/*.hex ExecutableFile.hex
#### Keil
find . -name "*.bak" | xargs rm -f 
find . -name "*.ddk" | xargs rm -f 
find . -name "*.edk" | xargs rm -f 
find . -name "*.lst" | xargs rm -f 
find . -name "*.lnp" | xargs rm -f 
find . -name "*.mpf" | xargs rm -f 
find . -name "*.mpj" | xargs rm -f 
find . -name "*.obj" | xargs rm -f 
find . -name "*.omf" | xargs rm -f 
find . -name "*.opt" | xargs rm -f 
find . -name "*.plg" | xargs rm -f 
find . -name "*.rpt" | xargs rm -f 
find . -name "*.__i" | xargs rm -f 
find . -name "*.crf" | xargs rm -f 
find . -name "*.o" | xargs rm -f 
find . -name "*.d" | xargs rm -f 
find . -name "*.axf" | xargs rm -f 
find . -name "*.tra" | xargs rm -f 
find . -name "*.dep" | xargs rm -f 
find . -name "JLinkLog.txt" | xargs rm -f 

find . -name "*.iex" | xargs rm -f 
find . -name "*.htm" | xargs rm -f 
find . -name "*.sct" | xargs rm -f 
find . -name "*.map" | xargs rm -f 
find . -name "*.scvd" | xargs rm -f 

find . -name "*.txt" | xargs rm -f 
find . -name "*.ini" | xargs rm -f 
find . -name "*.Administrator" | xargs rm -f 
find . -name "*.uvopt*" | xargs rm -f 
find . -name "*.uvgui*" | xargs rm -f 

rm -rf ./USER/DebugConfig

find . -name "*Objects*" | xargs rm -rf 
find . -name "*OBJ*" | xargs rm -rf 
find . -name "*Listings*" | xargs rm -rf 
