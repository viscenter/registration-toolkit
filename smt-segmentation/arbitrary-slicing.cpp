int slicing_start(int dd, float BoundingBoxX, float BoundingBoxY, float BoundingBoxZ, int sliceNo, floatVect *&oCorner){

	float voxelSizeZ = BoundingBoxZ/dd;
	oCorner = new floatVect[4];

	// Calculate the 3D coordinates of the 4 corners of the original slice
	oCorner[0].x = 0.0;
	oCorner[0].y = 0.0;
	oCorner[0].z = sliceNo * voxelSizeZ;

	oCorner[1].x = BoundingBoxX;
	oCorner[1].y = 0.0;
	oCorner[1].z = sliceNo * voxelSizeZ;

	oCorner[2].x = BoundingBoxX;
	oCorner[2].y = BoundingBoxY;
	oCorner[2].z = sliceNo * voxelSizeZ;

	oCorner[3].x = 0;
	oCorner[3].y = BoundingBoxY;
	oCorner[3].z = sliceNo * voxelSizeZ;

	return 0;

}

int slicing_rotateX(floatVect *&oCorner, float angle){

	
	// Calculate the 3D coordinates of the 4 corners of the slice after rotation around the X axis

	floatVect center;

	center.x = (oCorner[0].x + oCorner[1].x + oCorner[2].x +oCorner[3].x)/4;
	center.y = (oCorner[0].y + oCorner[1].y + oCorner[2].y +oCorner[3].y)/4;
	center.z = (oCorner[0].z + oCorner[1].z + oCorner[2].z +oCorner[3].z)/4;

	floatVect *rCorner = new floatVect[4];

	for (int i=0; i<4; i++){
		oCorner[i].x -= center.x;
		oCorner[i].y -= center.y;
		oCorner[i].z -= center.z;

		rCorner[i].x = oCorner[i].x;
		rCorner[i].y = oCorner[i].y * cos(angle) - oCorner[i].z * sin(angle);
		rCorner[i].z = oCorner[i].y * sin(angle) + oCorner[i].z * cos(angle);
		
		rCorner[i].x += center.x;
		rCorner[i].y += center.y;
		rCorner[i].z += center.z;
		
		oCorner[i].x = rCorner[i].x;
		oCorner[i].y = rCorner[i].y;
		oCorner[i].z = rCorner[i].z;

	}

	delete [] rCorner;
	return 0;
}


int slicing_rotateY(floatVect *&oCorner, float angle){

	// Calculate the 3D coordinates of the 4 corners of the slice after rotation around the Y axis

	floatVect center;

	center.x = (oCorner[0].x + oCorner[1].x + oCorner[2].x +oCorner[3].x)/4;
	center.y = (oCorner[0].y + oCorner[1].y + oCorner[2].y +oCorner[3].y)/4;
	center.z = (oCorner[0].z + oCorner[1].z + oCorner[2].z +oCorner[3].z)/4;

	floatVect *rCorner = new floatVect[4];

	for (int i=0; i<4; i++){
		oCorner[i].x -= center.x;
		oCorner[i].y -= center.y;
		oCorner[i].z -= center.z;

		rCorner[i].y = oCorner[i].y;
		rCorner[i].x = oCorner[i].x * cos(angle) + oCorner[i].z * sin(angle);
		rCorner[i].z = oCorner[i].x *(-1)* sin(angle) + oCorner[i].z * cos(angle);
		
		rCorner[i].x += center.x;
		rCorner[i].y += center.y;
		rCorner[i].z += center.z;

		oCorner[i].x = rCorner[i].x;
		oCorner[i].y = rCorner[i].y;
		oCorner[i].z = rCorner[i].z;
	}

	delete [] rCorner;
	return 0;
}


int slicing_rotateZ(floatVect *&oCorner, float angle){

	// Calculate the 3D coordinates of the 4 corners of the slice after rotation around the Z axis

	floatVect center;

	center.x = (oCorner[0].x + oCorner[1].x + oCorner[2].x +oCorner[3].x)/4;
	center.y = (oCorner[0].y + oCorner[1].y + oCorner[2].y +oCorner[3].y)/4;
	center.z = (oCorner[0].z + oCorner[1].z + oCorner[2].z +oCorner[3].z)/4;

	floatVect *rCorner = new floatVect[4];

	for (int i=0; i<4; i++){
		oCorner[i].x -= center.x;
		oCorner[i].y -= center.y;
		oCorner[i].z -= center.z;

		rCorner[i].z = oCorner[i].z;
		rCorner[i].x = oCorner[i].x * cos(angle) - oCorner[i].y * sin(angle);
		rCorner[i].y = oCorner[i].x * sin(angle) + oCorner[i].y * cos(angle);
		
		rCorner[i].x += center.x;
		rCorner[i].y += center.y;
		rCorner[i].z += center.z;

		oCorner[i].x = rCorner[i].x;
		oCorner[i].y = rCorner[i].y;
		oCorner[i].z = rCorner[i].z;
	}

	delete [] rCorner;
	return 0;
}

int slicing_translation(floatVect *&oCorner, int X, int Y, int Z){

	// Calculate the 3D coordinates of the 4 corners of the slice AFTER TRANSLATION 
	for (int i=0; i<4; i++){
		oCorner[i].x += X;
		oCorner[i].y += Y;
		oCorner[i].z += Z;
	}
	return 0;

}

int cal_slice_image(float *img1, int ww, int hh, int dd, float BoundingBoxX, float BoundingBoxY, float BoundingBoxZ, floatVect *rCorner, 
					float min, float max,unsigned char *&img2){

	img2 = new unsigned char[ww*hh];

	float voxelSizeX = BoundingBoxX/ww;
	float voxelSizeY = BoundingBoxY/hh;
	float voxelSizeZ = BoundingBoxZ/dd;
	
	float thresholdFactor = 255.0/(float)(max - min);
	// Calculate the slice image 
	floatVect direct30, direct21, p1, p2, p, directp2p1;
	vect voxInd;

	direct30.x = rCorner[3].x - rCorner[0].x;
	direct30.y = rCorner[3].y - rCorner[0].y;
	direct30.z = rCorner[3].z - rCorner[0].z;

	direct21.x = rCorner[2].x - rCorner[1].x;
	direct21.y = rCorner[2].y - rCorner[1].y;
	direct21.z = rCorner[2].z - rCorner[1].z;

	float t1=0.0;
	float t1Inc = 1.0/(hh-1);
	float t2Inc = 1.0/(ww-1);
	for (int i=0; i< hh; i++){
		p1.x = rCorner[0].x + direct30.x * t1;
		p1.y = rCorner[0].y + direct30.y * t1;
		p1.z = rCorner[0].z + direct30.z * t1;

		p2.x = rCorner[1].x + direct21.x * t1;
		p2.y = rCorner[1].y + direct21.y * t1;
		p2.z = rCorner[1].z + direct21.z * t1;

		directp2p1.x = p2.x - p1.x;
		directp2p1.y = p2.y - p1.y;
		directp2p1.z = p2.z - p1.z;
		
		float t2 = 0.0;
		for (int j=0; j< ww; j++){	
			p.x = p1.x + directp2p1.x * t2;
			p.y = p1.y + directp2p1.y * t2;
			p.z = p1.z + directp2p1.z * t2;
			float value;

			if ((p.x < 0.0) || (p.y < 0.0) ||(p.z < 0.0)
				||(p.x > BoundingBoxX) || (p.y > BoundingBoxY) ||(p.z > BoundingBoxZ)) {
				value = 0.0;
			}
			else {
				voxInd.x = (int) (p.x /voxelSizeX);
				voxInd.y = (int) (p.y /voxelSizeY);
				voxInd.z = (int) (p.z /voxelSizeZ);
				if (voxInd.x==ww) voxInd.x--;
				if (voxInd.y==hh) voxInd.y--;
				if (voxInd.z==dd) voxInd.z--;

				long offset1 = voxInd.z * hh * ww + voxInd.y * ww + voxInd.x;
				value = img1[offset1];
			}
			if (value<min) value = min;
			else if (value > max) value = max;
			value	-= min;
			value	= value * thresholdFactor;
			long offset2 = i*ww + j;
			img2[offset2] = value;
			t2 += t2Inc;
		}

		t1 +=t1Inc;
	}

	delete [] img2;

	return 0;
}