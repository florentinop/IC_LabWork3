To build:
	make

To test:
	bin/fcm Europarl/portuguese.utf8 -k 3 -a 1          // computes a fcm for the text stored in "Europarl/portuguese.utf8" and writes it to model_portuguese.utf8 
                                                        // using k=3 and alpha=1
                                            
    bin/lang model_greek.utf8 text_gk.txt -k 3 -a 1     // computes the bits required to compress "text_gk.txt", using the model "model_greek.utf8"
                                                        // using k=3 and alpha=1

    // the following commands must be executed in order!

    bin/findlang                                        // opens the findlang interface
    Europarl/german.utf8                                // adds the German model path to the models list
    german                                              // names the previous model as "german"
    Europarl/polish.utf8                                // adds the Polish model path to the models list
    polish                                              // names the previous model as "polish"
    ENTER                                               // stops the addition of models
    Europarl/german.utf8                                // indicates in which language "Europarl/german.utf8" was probably written (between German and Polish) 


    // the following commands must be executed in order!
    bin/locatelang                                        // opens the locatelang interface
    Europarl/german.utf8                                  // adds the German model path to the models list
    german                                                // names the previous model as "german"
    Europarl/polish.utf8                                  // adds the Polish model path to the models list
    polish                                                // names the previous model as "polish"
    ENTER                                                 // stops the addition of models
    Europarl/german_and_polish.utf8                       // indicates the starting positions of each language in german_and_polish.utf8 



To clean:
	make clean	