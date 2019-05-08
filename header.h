#include <iostream>
#include <cstdlib> // Pour la fonction rand()
#include <ctime>
#include <limits> // pour le paramètre de la fontion ignore()
#include <vector>

using namespace std ;

// ID de l'Auteur
void ID()
{
	cout << "		=================================================== " <<endl ;
	cout << "	  			SIRIKI KONE	"		  << endl ;
	cout << "			PROJET DE SIMULATION D'UN BASSIN DE TAMPON"	<< endl ;
	cout << "		=================================================== " <<endl<<endl ;
}

// Fonction de sortie du programme
void Exit()
{
	cout << "		============================================== " <<endl ;
	cout << "				FIN DE LA SIMULATION !!! "		  << endl ;
	cout << "		============================================== " <<endl ;
}

void continuer()
{
	char c ;

	cout << " appuyer sur 'c' pour continuer: " ; cin >> c ;
}


//  Pour confirmer un choix
bool confirmer()
{

	char choix ;

		do
		{
			cout << " choix (o/n): " ; cin >> choix ;

			if(choix == 'o' || choix == 'O')
				return true;

			else if(choix == 'n' || choix == 'N') return false ;


		} while(choix != 'o' && choix != 'O' && choix != 'n' && choix !='N' ) ;


}

// Contient les Caractéristiques d'un tampon
class tampon
{
	public:
		// donnee du bloc
		int Donnees ;

		// si le bloc est modifié (true) ou pas (false)
		bool DirtyBit ;

		// Nombre de fois que le bloc est utilisé
		size_t Usage ;

		// Numero du bloc dans le fichier
		int NumBloc ;

		// Si un bloc tampon est resevé ou pas
		bool Reserver ;

		tampon()
		{
			// On suppose que les donnée vont tous être positif, donc -1 pour dire qu'il n'y en aucun
			Donnees = -1 ;

			// Au chargement, le bloc est considéré intact
			DirtyBit = false ;

			// Bloc pas encore utilisé
			Usage = 0 ;

			//Tous les tampons sont considérés libres avant reservation
			Reserver = false ;

		}

		// initialiser un tampon
		void initialiser()
		{
			DirtyBit = false ;
			Reserver = false ;
			Usage = 0 ;
			NumBloc = -1 ;
			Donnees = -1 ;
		}
		
};

// Bassin de tampon
class Bassin
{
	private:
		tampon *Buffer ;

		// taille du bassin
		size_t size ;

		// Creation du fichier
		vector<int> fichier ;

	public:
		Bassin(size_t uneSize = 5)
		{
			size = uneSize;

			// le bassin contient 5 tampons
			Buffer = new tampon[size] ;

			// Initialisation du fichier, le premier element du tableau ne serait pas utilise
			for (int i = 0; i < 16 ; i++)
			 {
			 	// Ajouter des blocs au fichier (de 1 à 15)
			 	fichier.push_back(i) ;
			 }
		}


		// Obtenir numero de bloc le moins utilisé
		size_t getMin()
		{
			//cout << " %%%%%%%% GetMin() Method !!! %%%%%%%%" <<endl <<endl ;

			// On suppose que le premier tampon est le moins utilisé
			size_t min = Buffer[0].Usage ;
			size_t retourValue = 0 ;

			for (size_t i = 1; i < size ; i++)
			{
				if (Buffer[i].Usage < min)
				{
					// Un element moins utilisé trouvé
					min = Buffer[i].Usage ;
					retourValue = i ;
				}
			}

			return Buffer[retourValue].NumBloc ;
		}

		/* ============================= getPointer() ==================================== */

		// Obtenir un pointeur sur un bloc
		int getPointer(size_t numBloc)
		{
			//cout << " getPointer Method !!! " << endl << endl ;

			// Trouver le tampon qui contient le bloc
			for (int i = 0; i < 5 ; i++)
			{
				if (Buffer[i].NumBloc == numBloc)
				{
					// Renvoyer la position de l'element
					return i ;	
				}
			}

			// Non resevé
			return -1 ;
		}

		// Afficher la valeur d'un bloc
		void printPointer(size_t numBloc)
		{
			int pointer =  getPointer(numBloc) ;

			// Si le bloc ne se trouve pas en memoire ou si la donnée n'as pas été lue en mémoire
			if(pointer == -1 || Buffer[pointer].Donnees == -1) 
			{
				// Ne bloc ne se trouve pas en memoire
				cout << " _______________________________________________________________________________" <<endl <<endl ;

				cout << "  Le bloc #" << numBloc << " ne se trouve pas ou n'est pas encore charge en memoire " <<endl ;
				cout << " ______________________________________________________ ________________________" <<endl <<endl ;

				continuer() ;
				return ;
			}

			cout << " __________________________________________________________" << endl <<endl ;

			cout << "  Le bloc #" << numBloc << " contient " << Buffer[pointer].Donnees << endl ;

			cout << " _________________________________________________________ " << endl <<endl ;

			continuer() ;

		}

		// Si un bloc est reservé ou pas
		bool isReserved(size_t numBloc)
		{
			// getPointer retourne -1 si le bloc n'est pas réservé
			if(getPointer(numBloc) == -1) 
				return false ; // Bloc non reservé

			 return true ;
		}

		/* ============================= acquireBuffer() ==================================== */

		// Réserver un bloc
		void acquireBuffer(size_t numBloc)
		{
			if(isReserved(numBloc)) 
			{ 
				cout << " ____________________________________________" <<endl <<endl ;
				cout << " 	Le bloc #" << numBloc << " est deja reserver " <<endl ;
				cout << " ____________________________________________" <<endl <<endl ;


				continuer() ;
				 return ;
			}

			cout << " Reservation du bloc #" << numBloc << " en cours..... " << endl <<endl ;

			// Recherche de tampon libre
			for (int i = 0; i < 5 ; i++)
			{
				// Trouver un tampon non reserve  
				if (!Buffer[i].Reserver)
				{
					// Un tampon vide trouvée

					/*=================================================================================================
						 Si le tampon possède une donnée en mémoire, essayer de trouver un autre .
						 Pour essayer de conserver un bloc déjà utilisé en memoire tampon le plus longtemps possible
					===================================================================================================*/

					if (Buffer[i].Usage > Buffer[getPointer(getMin())].Usage) ; // Ne rien faire

					// Un bloc libre est trouvé
					else
					{ 
						/*==================================================================================================
							Pour limiter les accès disque, un bloc n'est ecrite en memoire que s'il doit être completement
							Liberer.
						===================================================================================================*/

						// Si le bloc a été modifié, sauvegarder la modification avant de le liberer
						if (Buffer[i].DirtyBit)
						{
							// Sauver la modification dans le fichier
							cout << "__________________________________________________________" <<endl <<endl ;

							cout << " Le bloc #" << Buffer[i].NumBloc << " a ete modifier en memoire TAMPON. " <<endl <<endl ;
							cout << " Voulez-vous ecrasez son contenue dans le FICHIER ?" <<endl ;
							cout << "__________________________________________________________" <<endl <<endl ;

							if (!confirmer())
							{
								return ;
							}

							fichier[Buffer[i].NumBloc] = Buffer[i].Donnees ;
							Buffer[i].DirtyBit = false ;
						}


						// Mémoriser le numero de bloc pour lequel le tampon a été reservé et initialiser les données du tampon
						Buffer[i].NumBloc = numBloc ;
						Buffer[i].Usage = 0 ;
						Buffer[i].Reserver = true ;
						Buffer[i].Donnees = -1 ;

						return ;
					}
				}
			}

			cout << " ________________________________" << endl <<endl ;
			cout << " 	Aucun tampon de disponible. " << endl ;
			cout << " ________________________________" << endl <<endl ;

			// Demande à l'utilisateur s'il veut en liberer une ou pas 
			char choix ;
			cout << " Voulez vous en liberer une ? (o/n) : " ; cin >> choix ;

			// L'utilisateur ne veut liberer aucun des tampons
			if(choix == 'n' || choix == 'N') 
			{ 
				cout << " ________________________________________________" << endl <<endl ;
				cout << " Vous ne Voulez liberer aucun des des tampons. " << endl <<endl 
					<< " Reservation impossible !!!" << endl << endl ;

				continuer() ;
				cout << " _______________________________________________" << endl <<endl ;
				return ;
			} 


			// Pas de tampon vide, il faut en libérer une
			// Hypotèse: Libérer le moins utilisé

			// trouver  le numero du bloc le moins utilisé
			int MinBloc = getMin() ;

			// Libérer le bloc
			releaseBuffer(MinBloc) ;

			// On peut maintenant effectuer la reservation
			acquireBuffer(numBloc) ; 


		}

		/* ============================= releaseBuffer()  ==================================== */

		// Liberer un bloc du fichier
		void releaseBuffer(int numBloc)
		{
			//cout << " releaseBuffer Method !!! " <<endl << endl ;

			// Cette valeur devrai changer si le bloc est resevé
			int BlocLocation = -1 ;

			// Trouver le tampon qui contient le bloc
			for (int i = 0; i < 5 ; i++)
			{
				if (Buffer[i].NumBloc == numBloc)
				{
					// Renvoyer la position de l'element
					BlocLocation = i ;	
				}
			}

			// Bloc non resever
			if (BlocLocation == -1)
			{
				// Le bloc ne se trouve pas en memoire vive
				cout << "  _____________________________________________________________"<<endl <<endl ;
				cout << "    Le bloc #" << numBloc << " n'a pas encore ete Reserver !  " <<endl ;
				cout << "  _____________________________________________________________"<<endl <<endl ;

				continuer() ;

				// On ne peut pas le Liberer
				return ;
			}

			/* ========== Bloc est reservé et la position est connue ========== */


			// On peut maintenant Liberer le bloc
			/* On declare le bloc non reservé et initialise le bit sale, et on conserve le bloc en memoire aussi longtemp que possible.
			   Le bloc ne serait complètement libérer qu'en cas de necessité c-à-d qu'il a le plus petit nombre d'usage.
			   */ 
			Buffer[BlocLocation].Reserver = false ;
			
		}

		/* ============================= readBloc() ==================================== */

		// Lire le contenue d'un bloc en tampon
		void readBloc(size_t numBloc)
		{

			int pointeur = getPointer(numBloc) ;
			if (pointeur == -1)
			{
				// ================ Reserver avant lecture ??? ====================

				cout << "  ____________________________________"<<endl <<endl ;
				cout << " 	Le bloc #"  << numBloc << " n'est pas reserver " << endl  ;
				cout << "  _____________________________________"<<endl <<endl ;

				continuer() ;
				return ;
			}

			// Le Bloc a ete modifié dans le tampon, demander à l'utilisateur s'il veut relire le bloc
			if (Buffer[pointeur].Donnees != -1)
			{
				cout << " ______________________________________________" <<endl <<endl ;
				cout << "  Le bloc a ete modifie en memoire tampon. " <<endl
					 << "  Voulez vous ecraser le contenu du tampon ? " <<endl ;
				cout << " ______________________________________________" <<endl <<endl ;

				// L'utilisateur decide d'Abandonner la lecture
				if(!confirmer())
				{
					cout << " ______________________________________________" <<endl <<endl ;
					cout << "  Vous avez decider d'Abandonner la lecture ! " << endl ;
					cout << " ______________________________________________" <<endl <<endl ;

					continuer() ;

					return ;
				}

			}

			Buffer[pointeur].Donnees = fichier[numBloc] ;
			Buffer[pointeur].Usage++ ; 


		}

		/* ============================= MakeDirty() ==================================== */

		// Changer la valeur du bit sale (le mettre a true)
		void makeDirty(size_t numBloc)
		{
			cout << " makeDirty Method !!! " << endl << endl ;

			if(getPointer(numBloc) !=-1 ) Buffer[getPointer(numBloc)].DirtyBit = true ;

			return ;

		}


		/* ============================= WriteBloc() ==================================== */

		void writeBloc(size_t numBloc)
		{
			// Il faut Obtenir la position du bloc dans la memoire vive pour y ecrire une valeur entre 10 et 20
			int BlocLocation = getPointer(numBloc);

			if (BlocLocation == -1 || Buffer[BlocLocation].Donnees == -1)
			{
				// Le bloc n'as encrore pas été charger en memoire
				cout << " _________________________________________________________"<<endl <<endl ;
				cout << "	Error Le bloc " << numBloc << "n'a pas encore ete charger en memoire" <<endl ;
				cout << " _________________________________________________________"<<endl <<endl ;

				/* ===== Dois je reserver le bloc, le charger en memoire pour ensuite y ecrire ??? ===== */

				continuer() ;
				return ;
			}

			srand(time(NULL)) ;
			Buffer[BlocLocation].Donnees = 10 + rand()%10 ;

			//cout << " %%%% La valeur " << Buffer[BlocLocation].Donnees << " a ete en RAM %%%%" <<endl<<endl ;

			Buffer[BlocLocation].Usage++ ;

			// Modification effectuer en memoire vive
			Buffer[BlocLocation].DirtyBit = true ;


		}

		// Afficher l'état du bassin
		void state()
		{
			cout << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Etat du Bassin %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl <<endl ;

			// État des tampons
			for (int i = 0; i < size ; i++)
			{
				cout << " Tampon #" << i+1 << " :  " << endl ; 
				cout << "   Donnees: " << Buffer[i].Donnees ;
				cout << "   Bit Sale: " << Buffer[i].DirtyBit ;
				cout << "   Utilisation: " << Buffer[i].Usage ; 
				cout << "   Reserver: " ; 
					if(Buffer[i].Reserver) { cout << "Oui" ; cout << "   #Bloc: " << Buffer[i].NumBloc ;}
					else {cout << "Non" ; if (Buffer[i].Donnees != -1) cout << "   #Bloc: " << Buffer[i].NumBloc ;}
					
				cout << endl <<endl;
			}

			 // État du fichier
			cout << " =======================================================================" <<endl <<endl ;
	 	 	cout << " Fichier: " ;
	 	 	// a partir du premier bloc du fichier 
			for (int i = 1; i < fichier.size() ; i++)
			 {
			 	// Afficher contenue du bloc i
			 	cout << fichier[i] ; if(i < 15) cout << ", " ;
			 }
			cout << endl <<endl ;
			cout << " =======================================================================" <<endl <<endl ;

			cout << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl <<endl ;

		}
};

// Menu
void Menu()
{
	cout << "	r: Reserver bloc 		f: Liberer bloc" << endl <<endl;
	cout << "	l: Lire bloc 			g: get Pointer" << endl << endl ;
	cout << "	e: Ecrire			q: quitter" << endl << endl;
}

void simulationStart()
{
	// Afficher ID du programmeur
	ID() ;

	// Allocation d'un bassin de tampon pour la simulation
	 Bassin unBassin ;

	 // choix de l'utilisateur
	 char choix ;


	 do
	 {
	 	size_t numBloc ;
	 	int temp = unBassin.getPointer(numBloc) ;

	 	 // etat du bassin
	 	 unBassin.state() ;

	 	 // Si l'utilisateur a saisi un entier ou pas. Pour gerer l'erreur avec les cin
	 	 bool isNumber ;

	 	 do
	 	 {
	 	 	isNumber = true ;

		 	// Lecture d'un numero de bloc
		 	cout << " Entrer un numero de bloc entre 1 et 15: "; 

		 	 cin >> numBloc ; 

		 	 // L'utilisateur a saisi un caractère et non un entier
		 	 if(!cin>>numBloc) 
		 	 {
		 	 	// Nettoyer le flux
		 	 	cin.clear() ;
		 	 	// Ignorer le caractère saisi
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				isNumber = false ;
		 	 }

		 	 cout << endl << endl ;

		 } while(numBloc < 1 || numBloc > 15 || !isNumber);

	 	// choix de l'operation à effectuer

	 	cout << " Quelle operation desirez-vous effectuer ?: " <<endl <<endl;
	 	// Afficher menu
	 		Menu() ;

	 	do
	 	{
	 		// Lecture du choix de l'utilisateur
	 		cout << " choix: " ;  cin >> choix ;

	 		 // Traitement du chois de l'utilisateur
	 		switch(choix)
	 		{
	 			case 'r':
	 			case 'R':
	 				// Reserver le bloc
	 	 			//cout << " Reservation du bloc " << numBloc << "..... "<< endl <<endl ;

		 	 		// Appele à la methode de réservation du bloc
		 	 		unBassin.acquireBuffer(numBloc) ;

		 	 		break ;

		 	 	case 'f':
		 	 	case 'F':
		 	 		// Libérer le bloc
	 	 			cout << " Liberation du bloc " << numBloc << "..... "<< endl <<endl ;

	 	 			// Appele à la methode de Libération du bloc
	 	 			unBassin.releaseBuffer(numBloc) ;

	 	 			break ;

	 	 		case 'l':
	 	 		case 'L':
	 	 			// Lire le bloc dans un tampon
	 	 
			 	 	// Appele à la methode de Lecture du bloc
			 	 	unBassin.readBloc(numBloc) ;

			 	 	break ;

			 	 case 'g':
			 	 case 'G':

			 	 	// afficher le contenue du bloc
			 	 	unBassin.printPointer(numBloc) ;

			 	 	break ;

			 	 case 'e':
			 	 case 'E':
			 	 	// ecrire dans un bloc
			 	 	//cout << " Ecriture dans le bloc " << numBloc << "....." <<endl <<endl ;
			 	 	unBassin.writeBloc(numBloc) ;
			 	 	break ;

			 	 case 'q':
			 	 case 'Q':
			 	 	// Quitter le programme
			 	 	Exit() ;

	 	 			return ;

	 	 		default:
	 	 			cout << " ________________________________"<<endl <<endl ;

	 	 			cout << "		Choix non Valide !" 		  <<endl;
	 	 			cout << " _________________________________"<<endl <<endl ;
	 		} 

	 	}  while( 
	 	 			(choix != 'e' && choix !='E') && (choix != 'r' && choix != 'R') && (choix != 'l' && choix !='L') &&
	 	 			 (choix != 'f' && choix !='F') && (choix != 'g' && choix !='G')
	 	 		) ;

	 } while(true) ;

}
