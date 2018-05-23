#! /bin/bash
dir=/njulk/berth/main
#seq=(sin.txt min.txt lin.txt)
seq=(min.txt)
#out=(s m l)
out=(m)
index=(50 60 70 80)
for ((j=0;j<1;j++));do
	for((i=0;i<4;i++));do
		#echo "${seq[$j]}: ${index[i]}: ${out[j]}_${index[i]}draw.txt: ${out[j]}_${index[i]}plan.txt"
		#echo "\n"
		/njulk/berth/main ${seq[$j]} ${index[i]} ${out[j]}_${index[i]}draw.txt ${out[j]}_${index[i]}plan.txt
	done
done
