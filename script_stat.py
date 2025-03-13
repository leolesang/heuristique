import os
import subprocess
import platform

instances_dir = "./Instances_MKP"
valeurs_references_file = "./Instances_MKP/valeurs_references_instances.txt"

def executer_heuristique(fichier_instance, param):

    if platform.system() == "Windows":
        executable = ".\\heuristique.exe"  # Windows
    else:
        executable = "./heuristique"  # Linux

    command = f"{executable} {fichier_instance} {param} STAT"
    
    print(f"Exécution de la commande : {command}")
    try:
        subprocess.run(command, shell=True)
    except Exception as e:
        print(f"Erreur lors de l'exécution de {command}: {e}")


fichiers_instances = [f for f in os.listdir(instances_dir) if f != "valeurs_references_instances" and f.endswith(".txt")]

for fichier in fichiers_instances:
    chemin_fichier = os.path.join(instances_dir, fichier)
    executer_heuristique(chemin_fichier, 20)
    executer_heuristique(chemin_fichier, 40)

print("Toutes les commandes ont été exécutées.")