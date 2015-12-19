
The pro file contains information to copy the required data next to the built app.

Open the .pro file in QtCreator, 
. choose the rigth Kit, and Configure Project
. check that a  CUSTOM DEPLOY STEP "make intall" is enabled:
	.Projects > Run > Deployment > Add Deploy Step > Make : Make arguments = "install"
