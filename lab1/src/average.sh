if [ "$#" -eq 0 ]; then
	echo "No input arguments"
	exit 1
fi

count=0
sum=0

for arg in "$@"; do
	count=$((count + 1))
	sum=$(echo "$sum + $arg" | bc)
done

average=$(echo "scale=2; $sum / $count" | bc)

echo "number of arguments: $count"
echo "average: $average"
