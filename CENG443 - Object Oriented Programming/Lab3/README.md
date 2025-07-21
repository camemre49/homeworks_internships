## To compile and run

```bash
javac -d ./out src/*.java -Xlint:unchecked
# Usage: java CSVProcessor <csv_file_name> <question_no>
java -cp out Main pre_data.csv 1
```