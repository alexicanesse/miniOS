#  MiniOS
## Projet d'architecture système Alexi et Jean.

### Heap memory manager 

Deux implémentations sont proposé. memory_legacy.c contient l'implémentation initiale réalisée. Il s'agit de l'implémentation correspondant à ce que le TP demande. `hm_malloc` et `hm_malloc` sont les versions non sécurisées. `hm_free` est l'implémentation de free. Ce heap manager est testé et des tests sont disponibles. Néanmoins, nous ne fournissons aucune garantie sur le fonctionnement de cette implémentation et nous ne considérons pas qu'elle fait parti de notre projet.

L'implémentation présente dans le fichier memory.c est considérée comme l'implémentation du projet. Les trois fonctions misent à disposition de l'utilisateurice sont `cls_malloc`, `cls_realloc` et `cls_free`. Ces fonctions correspondent à un heap manager sécurisé. Néanmoins, comme nous vous l'avons expliqué, ce heap manager ne reprend pas exactement le fonctionnement donné par le sujet de TP.  

__Fonctionnement de `cls_malloc`.__ Il existe un certains nombre de classes définie par la macro `MAX_CLASS_INDEX`. 



