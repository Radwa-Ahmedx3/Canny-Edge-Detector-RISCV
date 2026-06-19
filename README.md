# commands that help 
# ls ~   :   it helps to see which the home directory i have and it include the name of repo 
# riscv64-unknown-elf-g++ --version : يتحقق من الفيرجن الي محملينها 
# qemu-riscv64 --version : ///////
# git branch -a : show all the branches we have 
# git checkout feature/dana-image-gaussian-sobel ينقلني على برانش دانا علشان نشوف اكوادها
# ls src : to see the files in the src like : gaussian.cpp and any file .cpp this is the source file 
# find . -name "*.cpp" -o -name "*.h" | head -30       : show all .cpp & .h file    but show only the first 20 result 
# cat src/gaussian.cpp                  they all show the code inside each file 
# cat src/sobel.cpp
# cat include/gaussian.hpp
# cat include/image.hpp
# cat include/sobel.hpp
# cat Makefile
# cat tests/pipeline_test.cpp
# cat Makefile.nada
# cat nada_tests/nada_test.cpp
# cat host_test_simple.cpp

# git fetch origin  : بيوريني لو اصحابي عملو تغيريات في الريبو ومنزلتهاش عندي ولا ايه 
# git pull origin Nada_Tamer     :   يجيب آخر تحديثات من البرانش بتاعك على حيتهاب ويعملها ميرج مع النسخه المحليه 
# git log --oneline -10 :  بيعرض  اخر 10  عمليات ال commit الي حصلت 
# git clone https://github.com/Radwa-Ahmedx3/Canny-Edge-Detector-RISCV.git  :  لتحميل الريبو وبنحط عنوانها من جيت هاب 
# cd Canny-Edge-Detector-RISCV : للدخول جوا الريبو 
# git checkout Nada_Tamer : اروح لبرانشي
# git status      :        تأكدي إن كل حاجة متسجلة (commit) ومرفوعة (push)
# ./phase3_test  :   علشان اشغل البرنامج واشوف كل الاوتبوت 
# 
 
#
# git add nada_tests/phase3_test.cpp       علشان ارفع على جيت هاب 
# git commit -m "Add Phase 3 tests: Gaussian, Sobel, Direction, Magnitude with visual output"
# git push origin Nada_Tamer


######## nano nada_tests/phase3_test.cpp : علشان نفتح الملف ونعدل  

# python3 --version :  علشالن نتاكد ان البرنامج ده متحمل لان ده الي بنعمل بيه الصور 
# to open the pic from  Windows Explorer: { 
# \\wsl$\Ubuntu-24.04\home\nada_tamer\Canny-Edge-Detector-RISCV\nada_tests\   
