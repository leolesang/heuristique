# **Heuristique**

Ce projet est une plateforme pédagogique permettant aux étudiants de s'exercer sur des vulnérabilités web courantes, comme l'injection SQL, XSS, etc. Les exercices sont classés par niveau de difficulté le but étant de récupérer des flags pour valider les exercices.

## **🚀 Description**

Chaque exercice simule une vulnérabilité différente de l'OWASP Top 10 et permet aux utilisateurs de tester leurs compétences en exploitation de failles. L'étudiant peut avant de d'exercer apprendre les vulnérabilités sur les pages explications, un qcm est mit à disposition pour chaque type d'exercice. Si l'étudiant est bloqué il peut alors regarder la solution de l'exercice.

## **📦 Installation**

### Étapes d'installation

1. Clone le projet :

```bash
git clone https://github.com/leolesang/heuristique.git
cd heuristique
```

## **📖 Usage**

```bash
make
```

```bash
.\heuristique [Instance] [Temps] [Algo (VNS|VND)]
```

## **📂 Structure du Projet**

heuristique/
├──  Instance_MKP # Dossier des fichiers instances
├──  resultat # Dossier des résultats
├──  src # Dossier du programme
└── README.md     
└── Makefile      

## **🛠 Technologies**

![C](https://img.shields.io/badge/C-A8B9CC?logo=C&logoColor=white)
