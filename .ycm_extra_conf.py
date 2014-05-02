import os.path
execfile(os.path.expanduser("~/.vim/bundle/YouCompleteMe/cpp/ycm/.ycm_extra_conf.py"))

flags.extend([
'-I',
'src',
'-I',
'ext/src/googletest/include',
'-I',
'ext/src/asio/include'
])
