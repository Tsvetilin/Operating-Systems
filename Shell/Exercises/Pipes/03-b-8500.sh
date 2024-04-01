#За всяка група от /etc/group изпишете "Hello, <група>", като ако това е вашата група, напишете "Hello, <група> - I am here!".

cat /etc/group | cut -d ':' -f 1 | sed -E 's/^.*$/Hello, &/'
#or
cat /etc/group | awk -F ':' '{print "Hello, " $1}'
#or

cat /etc/group | cut -d ':' -f 3 | awk -v user_group_id=$(id -g) '$1 == user_group_id { print "Hello " $1 " - I am here!" } $1 != user_group_id { print "Hello " $1 }'