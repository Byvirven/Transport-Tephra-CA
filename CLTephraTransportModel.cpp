#include "CLTephraTransportModel.hpp"

// constructeur de la classe OpenCL
CLTephraTransportModel::CLTephraTransportModel(int width, int height, int nbPoint, int density, int speed) {
	// instancier une matrice pour l'image de taille cv::Size(width,height)
	// de type CV_8UC3 : 3 canaux pour RGB de la taille d'un octet
	// et remplir la matrice de brun (RGB = 70,141,189)
	img = cv::Mat(cv::Size(((width-1)*nbPoint)+width+(2*nbPoint),((height-1)*nbPoint)+height+(2*nbPoint)), CV_8UC3,cv::Scalar(255,255,255));
	// espacement entre les vecteurs initaux
	Epsilon = nbPoint;
	// densité initiales
	InitialeDensity = (float)density;
	// taille de la matrice
	automataDim = new int[4];
	automataDim[0] = speed;
	automataDim[1] = width;
	automataDim[2] = height;
	automataDim[3] = width*height;
	// générer les vecteurs initiaux
	CLTephraTransportModel::initAutomatonCells();
	// Récupérer les données concernant l'architecture du
	// système ou 'platform model' comme décrit dans la
	// documentation OpenCL au chapitre 3.1
	// Voir chapitre 2.1 de l'API OpenCL C++
	if (cl::Platform::get(&platforms) == CL_SUCCESS) {
		std::cout<<"Architecture compatible OpenCL."<<std::endl;
	} else {
		throw std::logic_error("Architecture non-compatible OpenCL");
	}
	
	// Définir le contexte OpenCL qui gérera les périphériques
	// OpenCL, les queues des commandes, la mémoire, les objets
	// ainsi que les kernel et leurs exécutions.
	// Voir chapitre 4.3 de la documentation OpenCL
	// et chapitre 2.3 de l'API OpenCL C++'
	// CL_DEVICE_TYPE_ALL pour générer un contexte
	// prenant en compte tous les périphériques compatibles OpenCL
	// CL_DEVICE_TYPE_GPU est le type générique pour les GPU
	// Voir chapitre 4.2 de la documentation OpenCL pour la 
	// liste des périphériques type
	context = cl::Context(CL_DEVICE_TYPE_ALL, NULL, NULL, NULL, &err);
	if (err == CL_SUCCESS) {
		std::cout<<"Création du contexte OpenCL réussie."<<std::endl;
	} else {
		throw std::logic_error("Création du contexte OpenCL échouée.");
	}

	// Récupérer la liste des propriété du contexte OpenCL
	// Voir chapitre 2.3 de l'API OpenCL C++
	context_properties = context.getInfo<CL_CONTEXT_PROPERTIES>();
	if (context_properties.size() != 0) {
		std::cout
		<<"Récupération des propriétés du contexte réussie."
		<<std::endl;
	} else {
		throw std::logic_error
		("Récupération des propriétés du contexte échouée.");
	}

	// Récupérer la liste du/des périphérique(s) compatible(s)
	// OpenCL disponible pour le contexte créé.
	// Voir chapitre 2.3 de l'API OpenCL C++
	context_devices = context.getInfo<CL_CONTEXT_DEVICES>();
	if (context_devices.size() != 0) {
		std::cout
		<<"Récupération de la liste des périphériques du contexte réussie."
		<<std::endl;
	} else {		
		throw std::logic_error
		("Récupération de la liste des périphériques du contexte échouée.");
	}
	// Récupérer l'identifiant attribué au premier GPU
	// découvert sur la plateforme
	for (int i=0;i<platforms.size();i++) {
		for (int j=0;j<context_devices.size();j++) {
			if (context_devices[j].getInfo<CL_DEVICE_TYPE>() == 4) {
				id_GPU = j;
				break;
			}
		}
	} 
	// Créer la queue d'exécution des commandes OpenCL
	// N'ayant qu'un seul périphérique sur la machine de test
	// le choix du périphériques est simplifié, donc on prend le 0
	// Tentative de gestion de l'identifiant du GPU sur une plateforme 
	// possédant plusieurs périphériques
	// Voir le chapitre 5.1 de la documentation OpenCL ainsi
	// que le chapitre 3.9 de l'API OpenCL C++
	queue = cl::CommandQueue(context, context_devices[id_GPU], 0, &err);
	if (err == CL_SUCCESS) {
		std::cout<<
		"Création de la queue d'exécution OpenCL réussie."
		<<std::endl;
	} else {
		throw std::logic_error
		("Création de la queue d'exécution OpenCL échouée.");
	}
	// créer les buffers mémoire
	inputDimension = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						4 * sizeof(int) // taille
						);
	inputX = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						automataDim[3] * sizeof(float) // taille
						);
	inputY = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						automataDim[3] * sizeof(float) // taille
						);
	inputZ = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						automataDim[3] * sizeof(float) // taille
						);
	inputDensity = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						automataDim[3] * sizeof(float) // taille
						);
	outputDensity = cl::Buffer(context, // contexte du buffer 
						CL_MEM_WRITE_ONLY, // mode
						automataDim[3] * sizeof(float) // taille
						);
	// instancier la fenêtre d'affichage
	//cv::namedWindow("Modèle de transport du tephra",CV_WINDOW_KEEPRATIO );
	// afficher l'image dans la fenêtre
	//cv::imshow("Modèle de transport du tephra", img);
}


void CLTephraTransportModel::run(int ms, int iter) {
	int key,i=0;
	writeMemoryOnce();
	// graine
        time_t start,end;
	time(&start);
	do{
		key = cvWaitKey(ms);
		if (key == 'q' or key == 1048689 or key == 1048603) break;
		writeMemory();
		execKernel();
		time(&end);
		readMemory();
		//densityToPixel();
		if (i == 1 or i == 2 or i==3 or i==5 or i == 10 or i==20 or i ==30 or i ==50 or i==60 or i==70 or i==80 or i==90 or i == 100 or i==200 or i==300 or i == 400 or i==500 or i==600 or i==700 or i==800 or i == 900 or i == 1000 or i==1500 or i == 3000 or i == 5000  or i == 10000) {
			printf("%i %2lf ",i,(double)difftime(end,start));
			std::cout<<i<<" : "<<(double)difftime(end,start)<<std::endl;
		/*	std::cout<<"densité au sol :"<<std::endl;			float totalSol = 0.0;
			for(int j=automataDim[3]-automataDim[1]+1;j<automataDim[3]-1;j++) {
				std::cout<<j<<" = "<<CLdensity[j]<< " ";
				totalSol += CLdensity[j];
			}
			std::cout<<std::endl;
			std::cout<<"densité hors automate";
			int k =0;
			float totalSuspension = 0.0;
			for(int j=0;j<automataDim[3];j+=automataDim[1]) {
				std::cout<<k++<<" = "<<CLdensity[j]+CLdensity[j+automataDim[1]-1]<<" ";
				totalSuspension += CLdensity[j]+CLdensity[j+automataDim[1]-1];
			}
			std::cout<<std::endl;
			std::cout<<"densité dans l'automate : "<<InitialeDensity-totalSol-totalSuspension<<std::endl<<" densité hors automate : "<<totalSuspension<<std::endl<<" densité au sol : "<<totalSol<<std::endl;
		*/}
	} while (i++<iter);
	
}

void CLTephraTransportModel::densityToPixel(void) {
	int pixel;
	for (int i=0;i<automataDim[3];i++) {
		pixel = (int)((CLdensity[i]/InitialeDensity)*0xffffff);
		img.data[(i*3)] = (CLdensity[i] == 0.0) ? 0xff :pixel<<4;
		img.data[(i*3)+1] = (CLdensity[i] == 0.0) ? 0xff :pixel<<2;
		img.data[(i*3)+2] = (CLdensity[i] == 0.0) ? 0xff : pixel;
	}
	cv::imshow("Modèle de transport du tephra", img);
}

// tirer un nombre aléatoire entre 2 bornes
float CLTephraTransportModel::random(float min, float max) {
	return min+(max-min)*fmod(rand(),1000.0)/1000.0;
}

void CLTephraTransportModel::generateAdvectionX(void) {
	// graine pour la génération des nombre aléatoire
	srand(1);
	// décroissance des vents en fonction de l'altitude
	float decreaseWind = ((float)automataDim[0]-((float)automataDim[0]/2.0))/
							(float)automataDim[2];
	for (int i=0;i<automataDim[2];i++) {
		float advectionVal = random(((automataDim[2]-i-1)*decreaseWind),
									 automataDim[0]-(i*decreaseWind));
		for(int j=0;j<automataDim[1];j++) {
			CLx[j+(i*automataDim[1])] = advectionVal;
		}
	}
}

void CLTephraTransportModel::initAutomatonCells(void) {
	// initialisation de l'automate
	CLdensity = new float [automataDim[3]];
	CLx = new float [automataDim[3]];
	CLy = new float [automataDim[3]];
	CLz = new float [automataDim[3]];
	// initialisation des cellules
	for(int i=0;i<automataDim[3];i++) {
		CLdensity[i] = 0.0;
		CLx[i] = 0.0;
		CLy[i] = 0.0;
		CLz[i] = -1.0;
	}
	// densité de particules initiale
	CLdensity[1] = InitialeDensity;
	// générer le champs d'advection en x
	CLTephraTransportModel::generateAdvectionX();
	// afficher points d'advection
	/*int position = 0;
	for (int i=Epsilon;i<img.rows;i+=Epsilon+1) {
		for (int j=Epsilon;j<img.cols;j+=Epsilon+1) {
			TephraTransportModel::drawVector(j,i,position);
			position++;
		}
	}*/
}


void CLTephraTransportModel::makeKernel(void) {
	// ouvrir le fichier source OpenCL
	std::ifstream sourceFile("TephraTransportKernel.cl");
	// lire fichier source
	std::string sourceCode(
						std::istreambuf_iterator<char>(sourceFile), //start
						(std::istreambuf_iterator<char>()) //eof
						);
	// Créer un objet OpenCL depuis la source -> Chapitre 3.5 C++
	cl::Program::Sources programObj(
							1,std::make_pair(sourceCode.c_str(),
							sourceCode.length()+1)
							);
	// associer la source au contexte -> Chapitre 3.5 C++
	program = cl::Program(context, programObj);
	// Compiler le kernel online (JIT) associé au contexte
	// pour le périphérique OpenCL prédéfini
	program.build(context_devices);
	// Créer le kernel
	kernel = cl::Kernel(program, "localProbability");
}

void CLTephraTransportModel::writeMemoryOnce(void) {
	
	// écrire le contenu de la matrice dans le buffer mémoire
	// et insérer ce dernier la queue d'exécution
	queue.enqueueWriteBuffer(inputDimension,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						4* sizeof(int) , // taille 
						automataDim, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	event.wait();
	queue.enqueueWriteBuffer(inputX,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						automataDim[3]* sizeof(float) , // taille 
						CLx, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	event.wait();
	queue.enqueueWriteBuffer(inputY,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						automataDim[3]* sizeof(float) , // taille 
						CLy, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	event.wait();
	queue.enqueueWriteBuffer(inputZ,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						automataDim[3]* sizeof(float) , // taille 
						CLz, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	event.wait();
	// Passer au kernel les buffers qui sont
	// arguments de ce dernier
	kernel.setArg(0, inputDimension);
	kernel.setArg(1, inputX);
	kernel.setArg(2, inputY);
	kernel.setArg(3, inputZ);
}
void CLTephraTransportModel::writeMemory(void) {
	
	queue.enqueueWriteBuffer(inputDensity,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						automataDim[3]* sizeof(float) , // taille 
						CLdensity, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	event.wait();
	kernel.setArg(4, inputDensity);
	kernel.setArg(5, outputDensity);
}

void CLTephraTransportModel::readMemory(void) {
	//float * density = new float[automataDim[3]];
	queue.enqueueReadBuffer(outputDensity, CL_TRUE, 0, automataDim[3]*sizeof(float), CLdensity);
	//for (int i=0;i<automataDim[3];i++) {
		//std::cout<<density[i]<<" ";
	//}
	//std::cout<<std::endl;
}

void CLTephraTransportModel::execKernel(void) {
	// exécuter le kernel
	queue.enqueueNDRangeKernel(kernel, // kernel associé à la queue
						cl::NullRange, // offset de départ (0,0,0)
						cl::NDRange(automataDim[3]), //nb de work-item
						cl::NDRange(1), // nb work-item par work-group
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	// attendre la fin de l'exécution du kernel
	event.wait();
}

CLTephraTransportModel::~CLTephraTransportModel(void) {
	queue.finish();
	queue.flush();
	//CLTephraTransportModel::displayCells();
	cv::destroyWindow("Modèle de transport du tephra");
	std::cout<< "byebye volcano"<<std::endl;
}

//fonctions de déboggage
void CLTephraTransportModel::displayCells(void) {
	for (int i=0;i<automataDim[3];i++) {
		std::cout<<"ligne "<<i/automataDim[1]<<" colonne "<<i%automataDim[1]<<" :: x,y,z=["<<CLx[i]<<","<<CLy[i]<<","<<CLz[i] <<"]"<<" densité = "<<CLdensity[i]<<std::endl;
		std::cout<<std::endl;
	}
}
