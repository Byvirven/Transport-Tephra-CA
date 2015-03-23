int west(int position, int width) {
	return (position%width != 0) ? position-1 : -1;
}

int east(int position, int width) {
	return ((position+1)%width != 0) ? position+1 : -1;
}

int north(int position, int width) {
	return (position-width >= 0) ? position-width : -1;
}

int south(int position, int width, int size) {
	return (position+width < size) ? position+width : -1;
}

int northwest(int position, int width) {
	return (position%width!=0 and position-width-1 >= 0) 
			? position-width-1 : -1;
}

int northeast(int position, int width) {
	return ((position+1)%width != 0 and position-width+1 >= 0)
			? position-width+1 : -1;
}

int southwest(int position, int width, int size) {
	return (position%width!=0 and position+width-1 < size)
			? position+width-1 : -1;
}

int southeast(int position, int width, int size) {
	return ((position+1)%width != 0 and position+width+1 < size)
			? position+width+1 : -1;
}

int defineQuadrant(float x, float y) {
	if (x > 0.0) {
		return (y >= 0.0) ? 1 : 4;
	} else if (x < 0.0) {
		return (y >= 0.0) ? 2 : 3;
	} else {
		return (y >= 0.0) ? 1 : 4;
	}
}

float p0(float u, float vi, float vi1, int v) {
	return  (1.0-((u*vi)/(v*v)))*
			(1.0-((u*vi1)/(v*v)));
}

float pi(float u, float vi, float vi1, int v) {
	return  ((u*vi)/(float)(v*v))*
			(1.0-((u*vi1)/(float)(v*v)));
}

float pi1(float u, float vi, float vi1, int v) {
	return  (1.0-((u*vi)/(float)(v*v)))*
			((u*vi1)/(float)(v*v));
}

float pi4(float u, float vi, float vi1, int v) {
	return  ((u*vi)/(float)(v*v))*
			((u*vi1)/(float)(v*v));
}

__kernel void localProbability(__global const int * Dim,
					__global const float * x,
					__global const float * y,
					__global const float * z,
					__global const float * densityIn,
					__global       float * densityOut) {
	
	int i = get_global_id(0); // identifiant global
	int pos[9] = {i,east(i,Dim[1]),northeast(i,Dim[1]), 
						north(i,Dim[1]),northwest(i,Dim[1]),
						west(i,Dim[1]),southwest(i,Dim[1],Dim[3]),
						south(i,Dim[1],Dim[3]),southeast(i,Dim[1],Dim[3])
						};
	int sel;
		
					
	automatmp[i].density] = (i%Dim[1] != 0 and (i+1)%Dim[1] != 0 and i<Dim[3]-Dim[1]) ? 0.0 : densityIn[i];
	for (int k=0;k<9;k++) {
		if (pos[k] != -1 and densityIn[pos[k]] > 0.0) {
				int quadrant[12] = {
						east(pos[k],Dim[1]),north(pos[k],Dim[1]),northeast(pos[k],Dim[1]), 
						north(pos[k],Dim[1]),west(pos[k],Dim[1]),northwest(pos[k],Dim[1]),
						west(pos[k],Dim[1]),south(pos[k],Dim[1],Dim[3]),southwest(pos[k],Dim[1],Dim[3]),
						south(pos[k],Dim[1],Dim[3]),east(pos[k],Dim[1]),southeast(pos[k],Dim[1],Dim[3])
						};
				sel = defineQuadrant(x[pos[k]],z[pos[k]]);
				if (pos[k] == i and pos[k]%Dim[1] != 0 and (pos[k]+1)%Dim[1] != 0 and pos[k]<Dim[3]-Dim[1]) {
					automatmp[i].density] +=  densityIn[pos[k]]*p0(x[pos[k]], (quadrant[((sel-1)*3)] != -1) ? x[quadrant[((sel-1)*3)]] : (sel%2 == 0 ) ? x[pos[k]] : 1.0 , (quadrant[((sel-1)*3)+1] != -1) ? x[quadrant[((sel-1)*3)+1]] : (sel%2 == 1) ? 1.0 : x[pos[k]],(float)Dim[0]);
				} else if (quadrant[((sel-1)*3)] == i and pos[k]%Dim[1] != 0 and (pos[k]+1)%Dim[1] != 0 and pos[k]<Dim[3]-Dim[1]) {
					 automatmp[i].density] +=  densityIn[pos[k]]*pi(x[pos[k]], (quadrant[((sel-1)*3)] != -1) ? x[quadrant[((sel-1)*3)]] : (sel%2 == 0 ) ? x[pos[k]] : 1.0 , (quadrant[((sel-1)*3)+1] != -1) ? x[quadrant[((sel-1)*3)+1]] : (sel%2 == 1) ? 1.0 : x[pos[k]],(float)Dim[0]);
				} else if (quadrant[((sel-1)*3)+1] == i and pos[k]%Dim[1] != 0 and (pos[k]+1)%Dim[1] != 0 and pos[k]<Dim[3]-Dim[1]) {
					 automatmp[i].density] +=  densityIn[pos[k]]*pi1(x[pos[k]], (quadrant[((sel-1)*3)] != -1) ? x[quadrant[((sel-1)*3)]] : (sel%2 == 0 ) ? x[pos[k]] : 1.0 , (quadrant[((sel-1)*3)+1] != -1) ? x[quadrant[((sel-1)*3)+1]] : (sel%2 == 1) ? 1.0 : x[pos[k]],(float)Dim[0]);
				} else if (quadrant[((sel-1)*3)+2] == i and pos[k]%Dim[1] != 0 and (pos[k]+1)%Dim[1] != 0 and pos[k]<Dim[3]-Dim[1]) {
					 automatmp[i].density] +=  densityIn[pos[k]]*pi4(x[pos[k]], (quadrant[((sel-1)*3)] != -1) ? x[quadrant[((sel-1)*3)]] : (sel%2 == 0 ) ? x[pos[k]] : 1.0 , (quadrant[((sel-1)*3)+1] != -1) ? x[quadrant[((sel-1)*3)+1]] : (sel%2 == 1) ? 1.0 : x[pos[k]],(float)Dim[0]);
				} else {
					automatmp[i].density] += 0.0;
				}
		} else {
			automatmp[i].density] +=0.0;
		}
	}
	automatmp[i].density] += 0.0;
}
//iG+((i%nbGroup)*get_num_groups(0))

/**/
