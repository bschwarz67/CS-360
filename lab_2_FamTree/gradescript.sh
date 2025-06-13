#!/bin/sh 

dir=/home/bryan/Desktop/PersonalCodeRepo/CS-360/lab_2_FamTree
ge="$dir/Gradescript-Examples"

if [ $# -ne 1 ]; then
  echo 'usage: gradescript problem_number (1-100)' >&2
  exit 1
fi

if [ "$1" -gt 0 -a "$1" -le 100 ]; then
  enum=`echo $1 | awk '{ printf "%03d\n", $1 }'`
else
  echo 'usage: gradescript problem_number (1-100)' >&2
  exit 1
fi
  
program=bin/famtree
correct_program=plank_sol_bin/famtree
  
if [ ! -f $program ]; then
  echo "Problem $enum is incorrect."
  echo ""
  echo "Your program $program does not exist."
  exit 0
fi

  rm -f tmp-$enum-test-stdout.txt tmp-$enum-test-stderr.txt tmp-$enum-correct-stdout.txt tmp-$enum-correct-stderr.txt tmp-$enum-grader.txt

./$correct_program < $ge/$enum.txt > tmp-$enum-correct-stdout.txt 2> tmp-$enum-correct-stderr.txt
$dir/$program< $ge/$enum.txt > tmp-$enum-test-stdout.txt 2> tmp-$enum-test-stderr.txt
call="./$program < $ge/$enum.txt"

err=`diff tmp-$enum-correct-stderr.txt tmp-$enum-test-stderr.txt | wc | awk '{ print $1 }'`
if [ `wc tmp-$enum-correct-stderr.txt | awk '{ print $1 }'` != 0 ]; then
  out=0
else
  $dir/bin/grader $ge/$enum.txt tmp-$enum-test-stdout.txt > tmp-$enum-grader.txt
  teg=`cat tmp-$enum-grader.txt`
  if [ "$teg" = "Ok" ]; then
    out=0
  else
    out=1
  fi
fi

if [ $out != 0 -o $err != 0 ]; then
  echo "Problem $enum is incorrect."
  echo ""
  if [ $out != 0 ]; then
    echo "Your standard output does not match the correct one."
  fi
  if [ $err != 0 ]; then
    echo "Your standard error does not match the correct one."
  fi
  echo ""
  echo "TEST:"
  echo ""
  echo "$call"
  echo ""
  echo "FILES:"
  echo ""
  echo "Your standard output is in tmp-$enum-test-stdout.txt."
  echo "Your standard error  is in tmp-$enum-test-stderr.txt."
  echo ""
  echo "A correct standard output is in tmp-$enum-correct-stdout.txt."
  echo "The correct standard error  is in tmp-$enum-correct-stderr.txt."
  echo "The results of the grading program are in tmp-$enum-grader.txt."
  echo ""
  echo "Look at correct files and your files, perhaps run 'diff -y' on them, and figure out your mistake."
  echo "Please remember to delete these files when you are finished."
else 
  echo "Problem $enum is correct."
  echo ""
  echo "Test: $call" 
  rm -f tmp-$enum-test-stdout.txt tmp-$enum-test-stderr.txt tmp-$enum-correct-stdout.txt tmp-$enum-correct-stderr.txt tmp-$enum-grader.txt
fi
