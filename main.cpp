//////////////////////////////////////////////////////////////////
//                                                              //
// Auteur      : Dos Santos Oliveira Marco                      //
// Date        : 16 mars 2012                                   //
// Sujet       : Projet de semestre OpenCL                      //
// Contenu     : Automate cellulaire : Réseau de transport      //
// Commentaire : Modéliser le transport/disperssion du tephra   //
// Enseignant  : Dr. Paul Albuquerque                           //
//                                                              //
//////////////////////////////////////////////////////////////////

#define __CL_ENABLE_EXCEPTIONS
#include "CLTephraTransportModel.hpp"
#include <iostream>
#include <cstring>


std::string usage(std::string appName) {
	return	"\nusage : "+appName+" width height points millisec iter densité velocity\n";
}

std::string man(void) {
	return	"\n* width    : nombre de cellules/pixel en largeur de l'automate\n"
			"* height   : nombre de cellules/pixel en hauteur de l'automate\n"
			"* points   : nombre de points à afficher entre chaque vecteur\n"
			"* millisec : taux de rafraîchissement en milliseconde\n"
			"* iter     : nombre d'itération en seconde avant arrêt de l'automate\n"
			"* density  : densité de particules initiales de l'automate\n"
			"* velocity : vitesse maximale admise par l'automate\n"
			"             (permet de calculer dX = v/dt où dt=1, donc dX = v)\n";
}

std::string error(int err) {
	switch(err) {
		case 1 :
			return "\nerreur : la largeur doit être un nombre entier positive\n"
				   "         strictement supérieur à 0\n";
		case 2 :
			return "\nerreur : la longueur doit être un nombre entier positive\n"
				   "         strictement supérieur à 0\n";
		case 3 :
			return 	"\nerreur : le nombre de points entre chaque vecteur\n"
					"         doit être un nombre entier\n";
		case 4 :
			return 	"\nerreur : le taux de rafraichissement en [ms] doit être\n"
					"         un nombre entier positive strictement supérieur\n"
					"         à 0\n";
		case 5 :
			return 	"\nerreur : le nombre d'itération en [s] doit être "
					"         un nombre entier positive strictement supérieur\n"
					"         à 0\n";
		case 6 :
			return 	"\nerreur : la densité de particules initiale doit être "
					"         un nombre entier positive strictement supérieur\n"
					"         à 0\n";
		case 7 :
			return 	"\nerreur : le vitesse maximale en [m/s] admise par\n"
					"         l'automate doit être un nombre entier positive\n"
					"         strictement supérieur à 0\n";
		default:
			return "\nerreur : le nombre d'arguments est incorrects\n";
	}
}

int main(int argc, char *argv[]){
	// vérifier si on a le bon nombre d'arguments
	if (argc != 8) {
		std::cerr<<error(0)<<usage(argv[0])<<man()<<std::endl;
		return 1;
	}
	// vérifier que les arguments ont le bon type 
	// et qu'il ne soit pas négatif
	for(int i=1;i<argc;i++) {
		for(int j=0;j<strlen(argv[i]);j++) {
			if (!isdigit(argv[i][j])) {
				std::cerr<<error(i)<<usage(argv[0])<<man()<<std::endl;
				return 1;
			}
		}
	}
	// vérifier que les valeurs soit strictement supérieur
	// à 0 exception faites du nombre de points entre chaque
	// vecteur qui peut être nul
	for(int i=1;i<argc;i++) {
		if ((atoi(argv[i]) <= 0 and i!=3)) {
			std::cerr<<error(i)<<usage(argv[0])<<man()<<std::endl;
			return 1;
		}
	}
	// démarrage
	try {
		std::cout<<"Welcome..."<<std::endl;
		CLTephraTransportModel Automata(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[6]),atoi(argv[7]));
		Automata.makeKernel();
		Automata.run(atoi(argv[4]),atoi(argv[5]));
	} catch (cl::Error err) {
		std::cerr << "ERROR: " << err.what() << "(" 
		<< err.err() << ")" << std::endl;
	}
	return 0;
}
