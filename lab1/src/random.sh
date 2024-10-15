if [ "$#" -ne 1 ]; then
	echo "Usage: $0 filename"
	exit 1
fi

FILE=$1

for i in $(seq 1 150); do
	RANDOM_NUMBER=$(od -An -N2 -i /dev/random | awk '{print $1 % 900 + 100}')
	echo $RANDOM_NUMBER >> "$FILE"
done

echo "Done!"
