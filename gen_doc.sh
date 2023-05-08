#!/bin/sh
################################################################################
GH_REPO_TOKEN=$1
GH_REPO_NAME=OpenEFI
GH_REPO_REF=github.com/openefi/OpenEFI.git
################################################################################
##### Setup this script and get the current gh-pages branch.               #####
echo 'Setting up the script...'
# Exit with nonzero exit code if anything fails
set -e

# Create a clean working directory for this script.
echo $PWD

# Get the current gh-pages branch
git clone -b gh-pages https://git@$GH_REPO_REF
cd ./OpenEFI

##### Configure git.
# Set the push default to simple i.e. push only the current branch.
git config --global push.default simple
# Pretend to be an user called GitHub CI.
git config user.name "GitHub CI"
git config user.email "github@github-ci.org"

# Remove everything currently in the gh-pages branch.
# GitHub is smart enough to know which files have changed and which files have
# stayed the same and will only update the changed files. So the gh-pages branch
# can be safely cleaned, and it is sure that everything pushed later is the new
# documentation.
rm -rf *

# Need to create a .nojekyll file to allow filenames starting with an underscore
# to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
# Presumably this is only needed when the SHORT_NAMES option in Rust is set
# to NO, which it is by default. So creating the file just in case.
echo "" > .nojekyll
echo "devdocs.openefi.tech" > CNAME

################################################################################
##### Generate the Rust code documentation and log the output.          #####
echo 'Generating Rust code documentation...'
cd ..
cargo doc --no-deps
mv target/thumbv7em-none-eabihf/doc/** ./OpenEFI
cd ./OpenEFI

################################################################################
##### Upload the documentation to the gh-pages branch of the repository.   #####
# Only upload if Rust successfully created the documentation.
# Check this by verifying that the html directory and the file html/index.html
# both exist. This is a good indication that Rust did it's work.
if [ -f "open_efi/index.html" ]; then

    echo 'Uploading documentation to the gh-pages branch...'
    echo "<meta http-equiv=\"refresh\" content=\"0; url=open_efi/app\">" > index.html

    # Add everything in this directory (the Rust code documentation) to the
    # gh-pages branch.
    # GitHub is smart enough to know which files have changed and which files have
    # stayed the same and will only update the changed files.
    git add --all

    # Commit the added files with a title and description containing the Travis CI
    # build number and the GitHub commit reference that issued this build.
    git commit -m "Deploy code docs to GitHub Pages build: ${GITHUB_RUN_ID}" -m "Commit: ${GITHUB_SHA}" || true

    # Force push to the remote gh-pages branch.
    # The ouput is redirected to /dev/null to hide any sensitive credential data
    # that might otherwise be exposed.
    git push --force "https://${GH_REPO_TOKEN}@${GH_REPO_REF}" > /dev/null 2>&1 || true
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    echo 'Warning: Not going to push the documentation to GitHub!' >&2
    exit 1
fi