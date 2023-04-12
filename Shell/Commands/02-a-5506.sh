#Изтрийте файловете в home директорията си по-нови от practice/01/f3
#(подайте на rm опция -i за да може да изберете само тези които искате да изтриете).

find ~ -newer practice/01/f3 -type f -exec rm -i {} \;

#or

find ~ -newer practice/01/f3 -type f -delete

#or

rm -i $(find ~ -type f -newer practice/01/f3)

#or


#second way it does not ask for confirmation to delete
