__kernel void substitute(__global char *originalVector, 
						 __global char *substitutedVector) {
	int gid = get_global_id(0);
	
	switch(originalVector[gid]){
		case 'A':	substitutedVector[gid] = 'G';
					break;
		case 'B':	substitutedVector[gid] = 'X';
					break;
		case 'C':	substitutedVector[gid] = 'S';
					break;
		case 'D':	substitutedVector[gid] = 'Q';
					break;
		case 'E':	substitutedVector[gid] = 'F';
					break;
		case 'F':	substitutedVector[gid] = 'A';
					break;
		case 'G':	substitutedVector[gid] = 'R';
					break;
		case 'H':	substitutedVector[gid] = 'O';
					break;
		case 'I':	substitutedVector[gid] = 'W';
					break;
		case 'J':	substitutedVector[gid] = 'B';
					break;
		case 'K':	substitutedVector[gid] = 'L';
					break;
		case 'L':	substitutedVector[gid] = 'M';
					break;
		case 'M':	substitutedVector[gid] = 'T';
					break;
		case 'N':	substitutedVector[gid] = 'H';
					break;
		case 'O':	substitutedVector[gid] = 'C';
					break;
		case 'P':	substitutedVector[gid] = 'V';
					break;
		case 'Q':	substitutedVector[gid] = 'P';
					break;
		case 'R':	substitutedVector[gid] = 'N';
					break;
		case 'S':	substitutedVector[gid] = 'Z';
					break;
		case 'T':	substitutedVector[gid] = 'U';
					break;
		case 'U':	substitutedVector[gid] = 'I';
					break;
		case 'V':	substitutedVector[gid] = 'E';
					break;
		case 'W':	substitutedVector[gid] = 'Y';
					break;
		case 'X':	substitutedVector[gid] = 'D';
					break;
		case 'Y':	substitutedVector[gid] = 'K';
					break;
		case 'Z':	substitutedVector[gid] = 'J';
					break;
		default:
					substitutedVector[gid] = originalVector[gid];
	}
}
