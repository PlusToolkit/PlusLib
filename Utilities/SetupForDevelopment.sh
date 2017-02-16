#!/usr/bin/env bash

# Run this script to set up development with git.
# For more information, see:
# http://www.slicer.org/slicerWiki/index.php/Documentation/Labs/DevelopmentWithGit
#

printErrorAndExit() {
  echo 'Failure during git development setup' 1>&2
  echo '------------------------------------' 1>&2
  echo '' 1>&2
  echo "$@" 1>&2
  exit 1
}

# Make sure we are inside the repository.
currentDir=$0
if [ -n "$currentDir" ]
then
  currentDir=`pwd`
fi

inPlusLib=`echo $currentDir | awk '/PlusLib$/ {print $currentDir}'`
inUtil=`echo $currentDir | awk '/[uU]tilities$/ {print $currentDir}'`

if [ -n "$inUtil" ]
then
  cd ..
fi

if test -d .git/.git; then
  printErrorAndExit "The directory '.git/.git' exists, indicating a
  configuration error.

Please 'rm -rf' this directory."
fi

# Make 'git pull' on master always use rebase.
git config branch.master.rebase true

# Make automatic and manual rebase operations to display a summary and stat
# display of changes merged in the fast-forward operation.
git config rebase.stat true

setup_user() {
  read -ep "Please enter your full name, such as 'John Doe': " name
  echo "Setting name to '$name'"
  git config user.name "$name"
  read -ep "Please enter your email address, such as 'john@email.com': " email
  echo "Setting email address to '$email'"
  git config user.email "$email"
}

if [ "$1" == "copyOnly" ]; then
  # Copy hooks
  echo cp hooks/commit-msg .git/hooks
  cp hooks/commit-msg .git/hooks
  exit 0
fi

# Logic to introduce yourself to Git.
gitName=$(git config user.name)
gitEmail=$(git config user.email)
if [ "$gitName" == "" ] || [ "$gitEmail" == "" ]; then
  setup_user
fi

# Loop until the user is happy with the authorship information
for (( ; ; ))
do
# Display the final user information.
gitName=$(git config user.name)
gitEmail=$(git config user.email)
echo "Your commits will have the following author:

$gitName <$gitEmail>
"
read -ep "Is the author name and email address above correct? [Y/n] " correct
if [ "$correct" == "n" ] || [ "$correct" == "N" ]; then
  setup_user
else
  break
fi
done


# Record the version of this setup so the developer can be notified that
# this script and/or hooks have been modified.
SetupForDevelopment_VERSION=1
git config hooks.SetupForDevelopment ${SetupForDevelopment_VERSION}
