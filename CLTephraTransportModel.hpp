#ifndef CL_TEPHRATRANSPORTMODEL_CLASS_H
#define CL_TEPHRATRANSPORTMODEL_CLASS_H

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <time.h>
#include <stdexcept>
#include <cv.h>
#include <highgui.h>


#define MAX_SPEED 256

class CLTephraTransportModel {
	public:
		// Constructeurs et destructeur
		// de la classe OpenCL
		CLTephraTransportModel(int width, int height, int nbPoint, int density, int speed); // constructeur
		~CLTephraTransportModel(void); // destructeur
		// fonctions
		float random(float min, float max);
		void run(int ms, int iter);
		void drawVector(int x, int y, int id);
		void initAutomatonCells(void);
		void generateAdvectionX(void);
		void densityToPixel(void);
		void copyDensity(void);
		// fonctions dédiées GPU
		void makeKernel(void);
		void writeMemoryOnce(void);
		void writeMemory(void);
		void execKernel(void);
		void readMemory(void);
		// fonctions de déboggage
		void displayCells(void);
	private:
		// platform model
		std::vector<cl::Platform> platforms;
		// contexte d'exécution
		cl::Context context;
		// propriétés du contexte
		std::vector<cl_context_properties> context_properties; 
		// périphériques du contexte
		std::vector<cl::Device> context_devices;
		// identifiant du GPU
		cl_device_type id_GPU;
		// queue d'exécution
		cl::CommandQueue queue;
		// programme
		cl::Program program;
		// kernel
		cl::Kernel kernel;
		// buffer mémoire
		cl::Buffer inputDimension;
		cl::Buffer inputX;
		cl::Buffer inputY;
		cl::Buffer inputZ;
		cl::Buffer inputDensity;
		cl::Buffer outputDensity;
		// gestion d'événement
		cl::Event event;
		/// gestion de clock
		cl_ulong startClock;
		//gestion d'erreur
		cl_int err; 
		// Matrice de l'image
		cv::Mat img;
		
				// dimensionnement de l'automate
		int * automataDim; 
		// densité initiale
		int InitialeDensity;
		// Position de vecteur d'advection
		int Epsilon;
		// automate cellulaire
		float * CLdensity;
		float * CLx;
		float * CLy;
		float * CLz;
};

#endif
