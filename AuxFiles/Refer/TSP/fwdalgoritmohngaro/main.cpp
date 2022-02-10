 
	//=========================================SOLVE PROBLEM ================================================================//    
	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	
	hungarian_init(&p, costMatrix, tamanho, tamanho, mode);
	raiz->solucao = hungarian_solve(&p);
//	getVectorSolution(tamanho, raiz->rota, p.assignment);
//	verifCiclo(&tamanho, raiz->rota, vCiclo, raiz->arcos); 
	hungarian_free(&p);
	//=======================================================================================================================//

