#!/usr/bin/env bash
cd out

git config user.name "Travis CI"
git config user.email "michiel.fortuin@gmail.com"

git clone "https://${GH_TOKEN}@${GH_REF}" master:gh-pages

git add .
git commit -m "Deploy to GitHub Pages"

#copy files
cd ..
publish_copyfiles.sh
cd out

git push --force "https://${GH_TOKEN}@${GH_REF}" master:gh-pages