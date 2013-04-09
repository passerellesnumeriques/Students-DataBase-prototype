Tips to use GitHub:
 - Become a developper: 
   - click on Fork => this will create a copy of the repository into your account
   - using Git client (like TortoiseGit), create a clone in your local computer
   => You have now the code on your local computer
 - Save your changes
   - once you modified a file, commit your modifications => this will commit on your LOCAL repository
   - then PUSH your modifications => this will put your modification on your GitHub account
   - if you want to propose your modifications, make a PULL REQUEST => This will ask PN to include your modifications
 - Get the latest version
   - after you fork the PN repository, you are working on the copy done into your account
   - if since the time you make the fork some modifications have been done in the PN repository, you need to update your own copy
   - For this:
     - On you local copy, in your computer, ADD A REMOTE STREAM:
	      - name: upstream
	      - URL: https://github.com/passerellesnumeriques/Students-DataBase.git
	 - Each time you want to get the latest version:
	      - PULL from remote repository UPSTREAM => this will take the latest version, into your local computer
	      - PUSH the new version into your GitHub account
	      => Your GitHub account is up to date with latest version of PN repository
