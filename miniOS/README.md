#  MiniOS
## Projet d'architecture système Alexi et Jean.

### Heap memory manager 

Deux implémentations sont proposées. memory_legacy.c contient l'implémentation initiale réalisée. Il s'agit de l'implémentation correspondant à ce que le TP demande. `hm_malloc` et `hm_malloc` sont les versions non sécurisées. `hm_free` est l'implémentation de free. Ce heap manager est testé et des tests sont disponibles. Néanmoins, nous ne fournissons aucune garantie sur le fonctionnement de cette implémentation et nous ne considérons pas qu'elle fait parti de notre projet.

L'implémentation présente dans le fichier memory.c est considérée comme l'implémentation du projet. Les trois fonctions misent à disposition de l'utilisateurice sont `cls_malloc`, `cls_realloc` et `cls_free`. Ces fonctions correspondent à un heap manager sécurisé. Néanmoins, comme nous vous l'avons expliqué, ce heap manager ne reprend pas exactement le fonctionnement donné par le sujet de TP.  

__Fonctionnement de `cls_malloc`.__ Il existe un certains nombre de classes définie par la macro `MAX_CLASS_INDEX`. La classe d'indice `i < MAX_CLASS_INDEX` est utilisée pour les allocations de taille comprise entre 2^i et 2^{i+1}.  
Un tableau contient les structures de données des classes. Icelui est initialisé lors du premier appel à `cls_malloc`. Ce tableau est stocké sur une page isolée et protégée par une _guard page_ avant et après.  
Pour chaque classes, `MAX_PAGE_CLASS` sont mmapées.  
Chaque classe possède une _free list_ contenant une liste d'espaces allouables. Cette liste est stockée sur `NUMBER_OF_PAGES_FOR_FREE_LIST` pages. CEtte macro doit avoir une valeur assez élevé car il peut y avoir beaucoup d'espaces libres à allouer. 


