# container

Before compiling make sure to create a new file system a put it in the same folder with the container.c file
To create a new file system you can use the buildroot tool wich is the tool i used
useful link : https://community.arm.com/developer/tools-software/oss-platforms/w/docs/499/build-a-buildroot-user-space

1 - first compile the container.c file : 
	gcc container.c -o container

2 - executing the code :
	sudo ./container
    
2.1 - Limitation de la memoire :
	pour limiter la memoire, vous pouver ajouter l'argument --memory= et vous specifier la valeur que vous voulez.
	exemple : sudo ./container --memory=520M pour limter la memoire a 520M

2.2 - Limitation des nombre maximale des processus :
	pour limiter la memoire, vous pouver ajouter l'argument --pids= et vous specifier la valeur que vous voulez.
	exemple : sudo ./container --pids=20 

2.3 - Limition su cpu :
	pour limiter la memoire, vous pouver ajouter l'argument --cpu= et vous specifier la valeur que vous voulez.
	exemple : sudo ./container --cpu=10
