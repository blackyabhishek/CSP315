Quo->currentPosition(OPP,tempx,tempy);
        v=100000000;
        if(myDepthIndex==-1)
        {
            myDepthIndex=depth-1;
            Quo->copyShortestPath(tempShortestPath[MY][depth-1],MY);
        }
        if(oppDepthIndex==-1)
        {
            oppDepthIndex=depth-1;
            Quo->copyShortestPath(tempShortestPath[OPP][depth-1],OPP);
        }
        if(!((Quo->playerID(OPP)==1 && d==31) || (Quo->playerID(OPP)==2 && d==32)))
        {
            for(int i=0;i<4 && !prune;i++)
            {
                moveTemp=Quo->movePlayer(OPP,i);
                if(moveTemp!=0)
                {
                    //cout<<"**** OPP :"<<depth<<" 0 ("<<i<<")\n";
                    Quo->updateShortestPath(OPP);
                    if(Quo->reachedGoal(OPP))
                    {
                        v=-1000000+evaluationFunction();
                        beta=v;
                        Quo->assignPosition(tempx,tempy,OPP);
                        Quo->assignPathlength(tempOppMin,OPP);
                        return v;
                    }
                    temp=alphaBetaPruning(depth-1,alpha,beta,!myTurn,myDepthIndex,-1);
                    //cout<<"#### OPP :"<<depth<<" 0 ("<<i<<")"<<temp<<" "<<v<<"\n";
                    v=(temp<v)?temp:v;
                    Quo->assignPosition(tempx,tempy,OPP);
                    Quo->assignPathlength(tempOppMin,OPP);
                    beta=(beta<v)?beta:v;
                    if(beta<=alpha)
                    {
                        prune=true;
                        break;
                    }
                    if(moveTemp==3 && Quo->jumpPlayer(OPP,i))
                    {
                        //cout<<"**** OPP :"<<depth<<" 0 ("<<i<<")\n";
                        Quo->updateShortestPath(OPP);
                        if(Quo->reachedGoal(OPP))
                        {
                            v=-1000000+evaluationFunction();
                            beta=v;
                            Quo->assignPosition(tempx,tempy,OPP);
                            Quo->assignPathlength(tempOppMin,OPP);
                            return v;
                        }
                        temp=alphaBetaPruning(depth-1,alpha,beta,!myTurn,myDepthIndex,-1);
                        //cout<<"#### OPP :"<<depth<<" 0 ("<<i<<")"<<temp<<" "<<v<<"\n";
                        v=(temp<v)?temp:v;
                        Quo->assignPosition(tempx,tempy,OPP);
                        Quo->assignPathlength(tempOppMin,OPP);
                        beta=(beta<v)?beta:v;
                        if(beta<=alpha)
                        {
                            prune=true;
                            break;
                        }
                    }
                }
            }
        }
