# xterm:              len x h +  x  +   y 
#                     130   40  800    600   

#set up cu window
#xterm
xterm -geometry 100x26+0+0 -e "sh zcom384;su" &

#set up debug window
xterm -geometry 130x10+0+400 &

#set up vmlinux.dump
#gvim vmlinux.dump -geometry 40x16+0+390 & 

#set up run script
gvim z_new -geometry 20x16+620+0 &
