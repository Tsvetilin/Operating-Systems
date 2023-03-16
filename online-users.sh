who | cut -d ' ' -f 1 | xargs -I{} grep {} /etc/passwd | cut -d ':' -f 1,5 |cut -d ',' -f 1 | sort | uniq | column -t -s ':'


#Save as alias
alias onlineUsers="who | cut -d ' ' -f 1 | xargs -I{} grep {} /etc/passwd | cut -d ':' -f 1,5 |cut -d ',' -f 1 | sort | uniq | column -t
-s ':' "
#call
onlineUsers