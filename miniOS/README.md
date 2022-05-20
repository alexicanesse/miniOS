#  MiniOS
## Projet d'architecture système Alexi et Jean.

### Heap memory manager 

Deux implémentations sont proposées. memory_legacy.c contient l'implémentation initiale réalisée. Il s'agit de l'implémentation correspondant à ce que le TP demande. `hm_malloc` et `hm_malloc` sont les versions non sécurisées. `hm_free` est l'implémentation de free. Ce heap manager est testé et des tests sont disponibles. Néanmoins, nous ne fournissons aucune garantie sur le fonctionnement de cette implémentation et nous ne considérons pas qu'elle fait parti de notre projet.

L'implémentation présente dans le fichier memory.c est considérée comme l'implémentation du projet. Les trois fonctions misent à disposition de l'utilisateurice sont `cls_malloc`, `cls_realloc` et `cls_free`. Ces fonctions correspondent à un heap manager sécurisé. Néanmoins, comme nous vous l'avons expliqué, ce heap manager ne reprend pas exactement le fonctionnement donné par le sujet de TP.  

Les allocations sont séparées par des classes dont les tailles sont des puissances de 2. Afin d'obtenir un bon éqquilibre en performance temporelle et performance en usage mémoire, nous plaçons des _guard page_ de manière fréquente et utilisons systématiquement des cannaries. Les cannaries sont obtenue à partir d'un hashage dépendant d'un _seed_ afin d'augmenter la sécurité.  



__Fonctionnement de `cls_malloc`.__ Il existe un certains nombre de classes définie par la macro `MAX_CLASS_INDEX`. La classe d'indice `i < MAX_CLASS_INDEX` est utilisée pour les allocations de taille comprise entre 2^i et 2^{i+1}.  
Un tableau contient les structures de données des classes. Icelui est initialisé lors du premier appel à `cls_malloc`. Ce tableau est stocké sur une page isolée et protégée par une _guard page_ avant et après.  
Pour chaque classes, `MAX_PAGE_CLASS` sont mmapées.  
Chaque classe possède une _free list_ contenant une liste d'espaces allouables. Cette liste est stockée sur `NUMBER_OF_PAGES_FOR_FREE_LIST` pages qui sont page guardées. Cette macro doit avoir une valeur assez élevé car il peut y avoir beaucoup d'espaces libres à allouer.  

   
Lors d'un appel à `cls_malloc`, si l'indixe correspondant à la taille alloué est strictement plus petit que `MAX_CLASS_INDEX`, la fonction renvoie le premier espace libre disponible et le marque comme utilisé. Si aucun espace est disponible, on alloue assez de pages pour contenir `MIN_NUMBER_OF_BLOCK_TO_ALLOC` espaces mémoires dans la zone dédiée à cette classe. On protège évidement cette zone par des _guard pages_.   
Lors d'un appel à `cls_malloc`pour une plus grosse zone mémoire, on mmap suffisement de pages en protegeant la zone par une _guard page_ avant et après la zone allouée. 

Le __fonctionnement de `cls_realloc`__ est annalogue.  




