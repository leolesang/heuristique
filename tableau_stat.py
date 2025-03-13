import csv

valeurs_reference = {
    "100M5_1": 24381, "100M5_11": 42757, "100M5_21": 59822,
    "250M5_1": 59312, "250M5_11": 109109, "250M5_21": 149665,
    "500M5_1": 120148, "500M5_11": 218428, "500M5_21": 295828,
    "100M30_1": 21946, "100M30_11": 40767, "100M30_21": 57494,
    "250M30_1": 56842, "250M30_11": 107755, "250M30_21": 150163,
    "500M30_1": 116056, "500M30_11": 218104, "500M30_21": 301675
}

def calculer_diff(reference, valeur_obtenue):
    return valeur_obtenue - reference

def calculer_amelioration(initial, meilleur_profit):
    if initial == 0:
        return 0
    amelioration = ((meilleur_profit - initial) / initial) * 100
    return round(amelioration, 2)

input_file = "stat/data.txt"
output_file = "stat/resultats.csv"

results = []

with open(input_file, "r") as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) < 7:
            continue
        
        initial = int(parts[0])
        ls_1flip = int(parts[1])
        ls_echange = int(parts[2])
        vnd = int(parts[3])
        vns = int(parts[4])
        cpu_time = int(parts[5])
        instance_path = parts[6]
        
        instance_name = instance_path.split("\\")[-1].split(".")[0]

        valeur_ref = valeurs_reference.get(instance_name, "N/A")

        # Calcul de la différence avec la valeur de référence pour chaque solution
        diff_initial = calculer_diff(valeur_ref, initial) if valeur_ref != "N/A" else "N/A"
        diff_ls_1flip = calculer_diff(valeur_ref, ls_1flip) if valeur_ref != "N/A" else "N/A"
        diff_ls_echange = calculer_diff(valeur_ref, ls_echange) if valeur_ref != "N/A" else "N/A"
        diff_vnd = calculer_diff(valeur_ref, vnd) if valeur_ref != "N/A" else "N/A"
        diff_vns = calculer_diff(valeur_ref, vns) if valeur_ref != "N/A" else "N/A"

        meilleur_profit = max(initial, ls_1flip, ls_echange, vnd, vns)
        amelioration = calculer_amelioration(initial, meilleur_profit) if valeur_ref != "N/A" else "N/A"
        

        results.append([
            instance_name, initial, ls_1flip, ls_echange, vnd, vns, cpu_time, valeur_ref, 
            diff_initial, diff_ls_1flip, diff_ls_echange, diff_vnd, diff_vns, 
            amelioration,
        ])

results_triees = sorted(results, key=lambda x: float(x[13]) if x[13] != "N/A" else -float('inf'), reverse=True)

try:
    with open(output_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["Instance", "Initial", "LS_1Flip", "LS_Echange", "VND", "VNS", "CPU Time", "Valeur Référence", 
                        "Diff Initial", "Diff LS_1Flip", "Diff LS_Echange", "Diff VND", "Diff VNS",
                        "Amélioration (%)"])
        writer.writerows(results_triees)

    print(f"Résultats enregistrés dans {output_file}")
except Exception as e:
    print(f"Erreur lors de l'écriture du fichier CSV : {e}")
    exit(1)