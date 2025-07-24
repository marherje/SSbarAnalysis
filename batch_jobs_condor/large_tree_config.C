// Configuración C++ para manejar TTrees grandes
#include <TSystem.h>
#include <TTree.h>
#include <TFile.h>

void configure_large_trees() {
    // Aumentar límites de memoria para TTrees
    TTree::SetMaxTreeSize(1800000000000LL); // 1TB límite
    
    // Configurar cache size para lectura eficiente
    TFile::SetCacheFileDir("/tmp/"); // Directorio para cache
    TFile::SetOpenTimeout(300); // Timeout en segundos
    
    // Configurar compresión
    ROOT::EnableImplicitMT(4); // Usar 4 threads para operaciones MT
    
    // Configurar memoria para histogramas
    gSystem->SetMaxVirtMemory(8000000000LL); // 8GB límite virtual
    
    cout << "Configuración para archivos grandes aplicada" << endl;
}

// Función para hacer merge manual de TTrees
void manual_tree_merge(vector<string> input_files, string output_file) {
    configure_large_trees();
    
    TFile *fout = new TFile(output_file.c_str(), "RECREATE");
    
    for (const auto& filename : input_files) {
        TFile *fin = TFile::Open(filename.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            cout << "Error abriendo: " << filename << endl;
            continue;
        }
        
        // Copiar todos los objetos del archivo
        TList *keys = fin->GetListOfKeys();
        TIter next(keys);
        TKey *key;
        
        while ((key = (TKey*)next())) {
            TObject *obj = key->ReadObj();
            if (obj) {
                fout->cd();
                obj->Write();
            }
        }
        
        fin->Close();
        delete fin;
    }
    
    fout->Close();
    delete fout;
}
