# MES-Project
A "clone and go" MES project template.

## Installation

First, clone the repository:
```shell
git clone https://github.com/menga-team/MES-Project   
mv MES-Project <your_project_name>
cd <your_project_name>
```

After completing this step, there are two options for you:

### 1. Set up as a local project

To use the template as a local project on your PC, simply delete the git folder and your're good to go:

```
rm -rf .git
```

### 2. Set up as a new git repository

To set up your project as a new repository, first create a git repository, then follow these steps:

```
git remote remove origin   
git remote add origin <your_repository_url>
git push -u origin/main
```

You might want to remove or change this README beforehand.

## Usage

Install [mvm](https://github.com/menga-team/mes-version-manager). This utility will help you initialize your project and keep it up-to-date, aside from
providing a simple interface to the mes and vmes compilation and flashing pipelines. All files needed by mvm are listed in the .gitignore file, so your
repositories will stay clean.

Next, configure this projects mesproj.json to suit your application. It consists of the following:

```json
{
  "mes_version": "<version>",
  "vmes_version": "<version>",
  "mbs_version": "<version>",
  "sd_information": {
    "name": "<title>",
    "author": "<name>",
    "version": [<major>,<minor>,<patch>],
    "icon": "<path-to-icon>.m3if",
    "game": "<path-to-binary>.bin"
  }
}
```

Mvm will update your project accoring to the version of the dependencies specified in mesproj.json.
If you don't particularely require an older commit, we suggest using "latest". The rest is information
needed when writing your game onto an SD card.

If this is your first MES project, check out the [documentation](https://github.com/menga-team/MES/wiki) on developement for the MES platform.
For a detailed explanation on the build system or mvm, check their respective repositories or the wiki.
