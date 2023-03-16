# Копирайте <РЕПО>/exercises/data/emp.data във вашата home директория.
cp /srv/fmi-os/exercises/data/emp.data .
# Посредством awk, използвайки копирания файл за входнни данни, изведете:

# - общия брой редове
awk 'END{print NR}' emp.data
#or
awk 'BEGIN{i=0} {i++} END{print i}' emp.data
#or
awk '{i+=1} END{print i}' emp.data

# - третия ред
awk 'NR==3 {print}' emp.data
#or
awk '{if(NR==3) print $0}' emp.data

# - последното поле от всеки ред
awk '{print $NF}' emp.data

# - последното поле на последния ред
awk 'END{print $NF}' emp.data

# - всеки ред, който има повече от 4 полета
awk 'NF > 4 {print}' emp.data
#or
awk '{if(NF > 4) print $0}' emp.data

# - всеки ред, чието последно поле е по-голямо от 4
awk '$NF > 4 {print}' emp.data
#or
awk '{if($NF > 4) print $0}' emp.data

# - общия брой полета във всички редове
awk '{print NF}' emp.data

# - броя редове, в които се среща низът Beth
awk '/Beth/ {print}' emp.data | awk 'END{print NR}'
#or
awk '/.*Beth.*/ {i+=1} END{print i}' emp.data

# - най-голямото трето поле и редът, който го съдържа
 awk 'BEGIN {max = 0; line = " "} {if ($3 > max) {max = $3; line = $0}} END {printf line" "max"\n"}' emp.data

# - всеки ред, който има поне едно поле
awk 'NF > 2' emp.data
#or
awk '{if(NF > 0) print $0}' emp.data

# - всеки ред, който има повече от 17 знака
awk 'length($0) > 17' emp.data

# - броя на полетата във всеки ред и самият ред
awk '{print "fields:" NF "\n" "line:" $0}' emp.data

# - първите две полета от всеки ред, с разменени места
awk '{print $2 "\t" $1}' data/emp.data

# - всеки ред така, че първите две полета да са с разменени места
awk '{{temp = $1; $1 = $2; $2 = temp} print}' emp.data
#or
awk '{temp = $2; $2 = $1; $1 = temp; print $0}' emp.data

# - всеки ред така, че на мястото на първото поле да има номер на реда
awk '{{ $1 = NR; } print}' emp.data

# - всеки ред без второто поле
awk '{for (i = 1; i <= NF; i += 1) if (i!=2) printf "%s ", $i; printf "\n"}' emp.data
#or
awk '{$2 = "\t"; print $0}' emp.data

# - за всеки ред, сумата от второ и трето поле
awk '{sum = $2 + $3; printf "%.2f\n", sum; sum = 0}' emp.data
#or
awk '{printf "%.2f\n", $2 + $3}' emp.data

# - сумата на второ и трето поле от всеки ред
awk 'BEGIN {sum = 0} {sum += $2 + $3} END {print sum}' emp.data
