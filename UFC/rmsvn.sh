 find . -name '.svn' -exec rm -rf {} \;
 find . -name '.deps' -exec rm -rf {} \;
 find . -name '.lib' -exec rm -rf {} \;
 find . -name '*.o' -exec rm {} \;
 find . -name '*.d' -exec rm {} \;
 find . -name '*.o.d' -exec rm {} \;
