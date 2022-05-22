#  MiniOS

Bienvenue dans le README du projet de Jean de Sainte Marie et Alexi Canesse. Ce projet inclue l'intégralité du contenue demandé. à l'exception de l'intégration de la partie uiodriver. Un protoype a été réalisé mais nous n'avons pas eu le temps de correctement l'intégrer au projet.


Quelques libertées ont étées prisent avec votre accord concernant le heap memory manager afin d'obtenir un heap memory manager qui nous semble meilleur.  

## Utilisation de la library

Le seul header file de la library que l'utilisateurice devrait importer est `miniOS.h`. Le comportement en cas d'utilisation d'une fonction qui n'est pas présente dans ce header file est non définie.  

Pour compiler un projet utilisant la library, pensez à include `miniOS.h` dans les fichiers utilisant des fonctions ou structures de données de la library et compilez avec l'option `-lminiOS` et l'option `-L/PATH_TO_THE_libminiOS.so_FILE`.


Executez la ligne suivante avant de lancer votre fichier compilé.
```sh
export LD_LIBRARY_PATH=/PATH_TO_THE_libminiOS.so_FILE 
```

## Compilation

Pour compiler la library, placez-vous à la racine du projet, dans le même dossier que le MakeFile et executez
```sh
make
```

Il est possible de compiler la library de manière à ce que tous les appels à `malloc`, `realloc` et `free` utilisent notre propre HMM. Il suffit de compiler de cette manière:
```sh
make WITH_OWN_HMM=1
```

## Testes

Le projet comporte plusieurs tests. Les tests représentent de simples exemples de bon fonctionnement: le code y est moins propre. Les commentaires permettent toute fois de comprendre le comportement attendue.  

Pour tester le scheduler il y a deux tests: un avec RR et un autre avec CFS. Pour les executer, placez-vous à la racine du projet, dans le même dossier que le MakeFile et executez
```sh
#Pour tester RR
make test_hm_RR 

#Pour tester CFS
make test_CFS
```

Pour tester le heap memory manager il y a plusieurs tests possibles. 
```sh
#Pour lancer tous les tests 
make test_hm_all

#Pour tester la version legacy 
make test_legacy

#Pour tester malloc, realloc et free en condition normale
make test_hm

#Pour verifier le bon fonctionnement des guard pages pour les grosses allocations 
make test_hm_guard_page

#Pour tester les cannaries
make test_hm_overflow
```

Il est possible de compiler la library de manière à ce que tous les appels à `malloc`, `realloc` et `free` utilisent notre propre HMM. Utiliser les tests des vCPUs avec notre propre HMM est un test ultime qui fonctionne! Pour essayer, executez
```sh
make test_hm_RR WITH_OWN_HMM=1
```

## Liste des fonctions disponibles 

```C
void run(void);
void config_scheduler(int quantum, enum scheduler_policies scheduler_policy);
int create_vCPU(int nbr_vCPU); /* returns 0 unless it fails. errno is set if it fails */
int destruct_vCPU(int nbr_vCPU); /* returns 0 unless it fails. errno is set if it fails */
int create_uThread(void (*func)(void), int argc, const char * argv[]); /* returns 0 unless it fails. errno is set if it fails */
void destruct_current_uThread(uThread* thread);
int yield(void);
void* cls_malloc(size_t size);
void* cls_realloc(void* ptr, size_t size);
void cls_free(void* ptr);

/* left for legacy */
void *hm_malloc(size_t size);
void *hm_realloc(void* ptr, long int size);
void hm_free(void *ptr);
```

## Fonctionnement 

### vCPU et scheduler 

Les vCPU sont des threads et les uThreads sont des contexts. Tous les quantums de temps, le thread principale, celui de l'utilisateurice, recoit un `SIGALRM. Lors de la reception de ce signal, le thread envoie un `SIGUSR1`a tous les vCPUs. Iceux executent alors les fonctions du scheduler pour re_scheduler la tache qui était en cours et pour savoir quoi scheduler ensuite. Si rien n'est schedulable, le vCPU idle jusqu'à la fin du quantum.  

L'implémentation a fait en sorte de prendre en compte l'ajout de nouveaux uThreads après le lancement des vCPUs, la créations de nouveaux vCPUs plus-tard, la suppressions de vCPUS durant le fonctionnement et le fait que les taches peuvent terminer. Quand une tache termine, une nouvelle est immédiatement schédulée.

### Heap memory manager 

Deux implémentations sont proposées. memory_legacy.c contient l'implémentation initiale réalisée. Il s'agit de l'implémentation correspondant à ce que le TP demande. `hm_malloc` et `hm_malloc` sont les versions non sécurisées. `hm_free` est l'implémentation de free. Ce heap manager est testé et des tests sont disponibles. Néanmoins, nous ne fournissons aucune garantie sur le fonctionnement de cette implémentation et nous ne considérons pas qu'elle fait parti de notre projet.

L'implémentation présente dans le fichier memory.c est considérée comme l'implémentation du projet. Les trois fonctions misent à disposition de l'utilisateurice sont `cls_malloc`, `cls_realloc` et `cls_free`. Ces fonctions correspondent à un heap manager sécurisé. Néanmoins, comme nous vous l'avons expliqué, ce heap manager ne reprend pas exactement le fonctionnement donné par le sujet de TP.  

Les allocations sont séparées par des classes dont les tailles sont des puissances de 2. Afin d'obtenir un bon équilibre en performance temporelle et performance en usage mémoire, nous plaçons des _guard page_ de manière fréquente et utilisons systématiquement des cannaries. Les cannaries sont obtenue à partir d'un hashage dépendant d'un _seed_ généré aléatoirement afin d'augmenter la sécurité.  



__Fonctionnement de `cls_malloc`.__ Il existe un certains nombre de classes définie par la macro `MAX_CLASS_INDEX`. La classe d'indice `i < MAX_CLASS_INDEX` est utilisée pour les allocations de taille comprise entre 2^i et 2^{i+1}. Et celle d'indice `MAX_CLASS_INDEX`gère les plus grosses allocations.    
Un tableau contient les structures de données des classes. Icelui est initialisé lors du premier appel à `cls_malloc`. Ce tableau est stocké sur une page isolée et protégée par une _guard page_ avant et après.  
Pour chaque classes, `MAX_PAGE_CLASS` sont mmapées.  
Chaque classe possède une _free list_ contenant une liste d'espaces allouables. Cette liste est stockée sur `NUMBER_OF_PAGES_FOR_FREE_LIST` pages qui sont page guardées. Cette macro doit avoir une valeur assez élevé car il peut y avoir beaucoup d'espaces libres à allouer.  

   
Lors d'un appel à `cls_malloc`, si l'indice correspondant à la taille alloué est strictement plus petit que `MAX_CLASS_INDEX`, la fonction renvoie le premier espace libre disponible et le marque comme utilisé. Si aucun espace est disponible, on alloue assez de pages pour contenir `MIN_NUMBER_OF_BLOCK_TO_ALLOC` espaces mémoires dans la zone dédiée à cette classe. On protège évidement cette zone par des _guard pages_.   
Lors d'un appel à `cls_malloc`pour une plus grosse zone mémoire, on mmap suffisement de pages en protegeant la zone par une _guard page_ avant et après la zone allouée. 

__Fonctionnement de `cls_realloc`.__ Si le pointeur donné en entré est `NULL`, cette fonction se comporte comme `cls_malloc`. Sinon, on vérifie que le pointeur a bien été alloué. Cette fonction ne fait rien si la zone à reallouer est déjà de la bonne taille et fait des appels à `cls_malloc`et `cls_free` suivit d'un appel à `memcpy` sinon. Cette fonction vérifie aussi le cannaries lorsque cela a un sens.   

__Fonctionnement de `cls_free`.__ Cette fonctione vérifie que le pointeur donné en argument est a bien été alloué (protection contre les _double free_ attaque) et vérifie le cannaries si cela est nécessaire.   

La vérification des allocations se fait à l'aide d'une _used_list_. Icelle est placé au début des pages allouées à la _free list_ associées et croit vers le haut. Ceci a été décidé afin de réduire la consommation en mémoire de la library.




