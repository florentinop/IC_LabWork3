To build:
	make

To test:
	bin/fcm Europarl/portuguese.utf8 -k 3 -a 1          // computes a fcm for the text stored in "Europarl/portuguese.utf8" and writes it to model_portuguese.utf8 
                                                        // using k=3 and alpha=1
                                            
    bin/lang model_greek.utf8 text_gk.txt -k 3 -a 1     // computes the bits required to compress "text_gk.txt", using the model "model_greek.utf8"
                                                        // using k=3 and alpha=1

    


To clean:
	make clean	