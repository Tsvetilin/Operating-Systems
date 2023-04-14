#Намерете колко физическа памет заемат осреднено всички процеси на потребителската група root. Внимавайте, когато групата няма нито един процес.

ps -A -o drs,group | awk '{if ($2=="root") print $1}' | awk '{sum += $1; counter += 1;} END{if (counter > 0) print (sum/counter); else print "0"}'
