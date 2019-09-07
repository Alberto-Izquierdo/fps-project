tests=($(ls tst_*))
for test in ${tests}
do
    ./${test}
end
