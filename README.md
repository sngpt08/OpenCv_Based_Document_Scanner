# OpenCv_Based_Document_Scanner

1. create the build dir.
2. configure the OpenCV_DIR variable in CMakeList.txt.
3. Now follow the following cmd:-
     i. cd build
    ii. cmake ..
   iii. make
4. Now if everything is fine is will generate the binary with name "document_scanner".
5. execute the bin.
6. in test_data folder there is already two images which is used for test purpose.
7. when you execute the binary it will open a window. Make a mouse left double click on it.
8. it will read the image from test_data folder and will show the blue rectangle. you can drag and drop the vertices of rectangle.
9. press "o" to get the output image.
10. when you again make a mouse double click it will load the other image from test_data dir.
