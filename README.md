## Parallel Matrix Multiplication 

### Problema
Sviluppare un programma C che utilizza MPI per fornire una soluzione al seguente problema: Moltiplicando la matice A di grandezza m x n con la matrice B di grandezza n x l si ottiene la matrice C di grandezza m x l.

### Soluzione
La soluzione da me proposta prevede che il master non partecipi a calcolare le righe della matrice C, ma aspetti le righe calcolate dai worker e assembli la matrice C. Dunque se il numero di processori è 1, ovvero il master, quest'ultimo effettua l'operazione in modo sequenziale. 
Se il numero di processori, invece, è superiore ad 1, il master divide il numero di righe della matrice A per il numero di processori. Ad ogni processore viene inviato, dal master, un vettore che contiene il numero della riga da dove deve iniziare a computare e il numero delle riga dove fermarsi. In caso in cui N (il numero di righe di A) non è divisibile per p (il numero di processori), qualche worker avrà delle righe in più da computare.

Ogni worker calcola la sottomatrice di C in base alle righe che ha e rispedisce al master il risultato ed il vettore in cui sono contenute le informazioni sul numero della riga iniziale ed il numero della riga finale. 

Il master riassembla la matrice C e stampa il risultato.


### Strong Scaling
Questo test è stato effettuato su un numero crescente di istanze (da 1 a 8) StarCluster AMI ami-52a0c53b (Linux Ubuntu), dove, fissata la taglia della matrice a 3100x3100, ad ogni istanza aggiunta si aumentava il numero di processori nell'esecuzione.

![Grafico](img/Strong.png)

### Weak Scaling
Questo test è stato effettuato su un numero crescente di istanze (da 1 a 8) StarCluster AMI ami-52a0c53b (Linux Ubuntu), dove, fissata la taglia di 100 righe per ogni processore, ad ogni istanza aggiunta si aumentava il numero di processori nell'esecuzione e quindi si aumentava la taglia della matrice di 100.

![Grafico](img/Weak.png)

### Compilazione
```
mpicc namefile.c -o namefile
```

### Esecuzione
```
mpirun -np n_processors --host MASTER,IP_SLAVE1,IP_SLAVE2 namefile
```