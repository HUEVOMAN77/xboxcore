// Text the recomp adds in French, German, Italian, Spanish and Indonesian: Double Agent recomp's wording where the
// English is the same, in sentence case with German's nouns capitalized; the panel's rows and the game's menu show it
// in capitals. Indonesian has no game strings on any disc, so only this text changes with it.
#include <algorithm>
#include <cctype>
#include <iterator>
#include <map>
#include <mutex>

#include "common/options.h"
#include "common/translations.h"

namespace {

// in game::kLanguages order after int; an empty one (the touch rows the other languages lack) stays English
struct Entry {
    std::string_view english;
    std::u16string_view text[5];
};
constexpr Entry kEntries[] = {
    // the settings pages and their sections
    {"GENERAL", {u"Général", u"Allgemein", u"Generale", u"General", u"Umum"}},
    {"DISPLAY", {u"Affichage", u"Anzeige", u"Video", u"Pantalla", u"Tampilan"}},
    {"GRAPHICS", {u"Graphismes", u"Grafik", u"Grafica", u"Gráficos", u"Grafis"}},
    {"PRESS A KEY", {u"Appuyez sur une touche", u"Taste drücken", u"Premi un tasto", u"Presiona una tecla", u"Tekan tombol"}},
    {"UNBOUND", {u"Non assigné", u"Nicht belegt", u"Non assegnato", u"Sin asignar", u"Belum diatur"}},
    {"TOUCH", {{}, {}, {}, {u"Táctil"}, u"Sentuh"}},
    {"DRAG TO MOVE, PINCH OR - + TO RESIZE", {{}, {}, {}, {u"Arrastra para mover, pellizca o - + para cambiar el tamaño"},
                                              u"Seret untuk memindah, cubit atau - + untuk mengubah ukuran"}},
    {"QUALITY", {u"Qualité", u"Qualität", u"Qualità", u"Calidad", u"Kualitas"}},
    {"PERFORMANCE", {u"Performances", u"Leistung", u"Prestazioni", u"Rendimiento", u"Performa"}},
    {"TOUCH CONTROLS", {u"Commandes tactiles", u"Touch-Steuerung", u"Comandi touch", u"Controles táctiles", u"Kontrol sentuh"}},
    {"UPDATES", {u"Mises à jour", u"Updates", u"Aggiornamenti", u"Actualizaciones", u"Pembaruan"}},
    {"CONTROLS", {{}, {}, {}, {u"Controles"}, u"Kontrol"}},

    // the settings rows
    {"RESOLUTION", {u"Résolution", u"Auflösung", u"Risoluzione", u"Resolución", u"Resolusi"}},
    {"RENDER RESOLUTION", {u"Résolution de rendu", u"Render-Auflösung", u"Risoluzione di rendering", u"Resolución de renderizado", u"Resolusi render"}},
    {"VERTICAL SYNC", {u"Synchro verticale", u"V-Sync", u"Sincronia verticale", u"Sincronismo vertical", u"Sinkronisasi vertikal"}},
    {"MAX FRAME RATE", {u"Images par seconde max.", u"Max. Bildrate", u"Frame rate massimo", u"Cuadros por segundo máx.", u"Frame rate maks."}},
    {"FOG", {u"Brouillard", u"Nebel", u"Nebbia", u"Niebla", u"Kabut"}},
    {"CAR REFLECTIONS", {u"Reflets des voitures", u"Fahrzeugreflexionen", u"Riflessi delle auto", u"Reflejos de los autos", u"Pantulan mobil"}},
    {"FULL", {u"Complet", u"Voll", u"Pieno", u"Completo", u"Penuh"}},
    {"HALF", {u"Moitié", u"Halb", u"Metà", u"Mitad", u"Setengah"}},
    {"QUARTER", {u"Quart", u"Viertel", u"Un quarto", u"Un cuarto", u"Seperempat"}},
    {"ROAD REFLECTIONS",{u"Reflets de la route", u"Straßenreflexionen", u"Riflessi della strada", u"Reflejos de la carretera", u"Pantulan jalan"}},
    {"SHADOWS",{u"Ombres", u"Schatten", u"Ombre", u"Sombras", u"Bayangan"}},
    {"PARTICLE EFFECTS", {u"Effets de particules", u"Partikeleffekte", u"Effetti particellari", u"Efectos de partículas", u"Efek partikel"}},
    {"FALLING RAIN", {u"Pluie", u"Regen", u"Pioggia", u"Lluvia", u"Hujan turun"}},
    {"SCREEN RAINDROPS", {u"Gouttes de pluie à l'écran", u"Regentropfen auf dem Bildschirm", u"Gocce di pioggia sullo schermo", u"Gotas de lluvia en pantalla", u"Tetes hujan di layar"}},
    {"SHADER PRELOADING", {u"Préchargement des shaders", u"Shader vorladen", u"Precaricamento shader", u"Precarga de shaders", u"Muat shader di awal"}},
    {"MOTION BLUR", {u"Flou de mouvement", u"Bewegungsunschärfe", u"Sfocatura di movimento", u"Desenfoque de movimiento", u"Blur gerakan"}},
    {"YELLOW TINT", {u"Teinte jaune", u"Gelbstich", u"Tinta gialla", u"Tinte amarillo", u"Filter kuning"}},
    {"BLOOM", {u"Flou lumineux", u"Bloom", u"Bloom", u"Resplandor", u"Bloom"}},
    {"ASPECT RATIO", {{}, {}, {}, {u"Relación de aspecto"}, u"Rasio aspek"}},
    {"FULL SCREEN", {{}, {}, {}, {u"Pantalla completa"}, u"Layar penuh"}},
    {"SCREEN FILTER", {{}, {}, {}, {u"Filtro de pantalla"}, u"Filter layar"}},
    {"SHARPEN", {{}, {}, {}, {u"Nítido"}, u"Pertajam"}},
    {"VIVID", {{}, {}, {}, {u"Colores vivos"}, u"Warna cerah"}},
    {"SCANLINES", {{}, {}, {}, {u"Líneas de barrido"}, u"Garis scan"}},
    {"FPS COUNTER POSITION", {u"Position du compteur", u"Position der FPS-Anzeige", u"Posizione contatore FPS", u"Posición del contador", u"Posisi penghitung FPS"}},
    {"SHOW CPU / GPU USAGE", {u"Afficher l'utilisation CPU / GPU", u"CPU- / GPU-Auslastung anzeigen", u"Mostra utilizzo CPU / GPU", u"Mostrar uso de CPU / GPU", u"Tampilkan penggunaan CPU / GPU"}},
    {"SHOW TEMPERATURES", {u"Afficher les températures", u"Temperaturen anzeigen", u"Mostra temperature", u"Mostrar temperaturas", u"Tampilkan suhu"}},
    {"TOP CENTER", {u"En haut au centre", u"Oben mittig", u"In alto al centro", u"Arriba al centro", u"Atas tengah"}},
    {"TOP LEFT", {u"En haut à gauche", u"Oben links", u"In alto a sinistra", u"Arriba a la izquierda", u"Kiri atas"}},
    {"TOP RIGHT", {u"En haut à droite", u"Oben rechts", u"In alto a destra", u"Arriba a la derecha", u"Kanan atas"}},
    {"BOTTOM LEFT", {u"En bas à gauche", u"Unten links", u"In basso a sinistra", u"Abajo a la izquierda", u"Kiri bawah"}},
    {"BOTTOM CENTER", {u"En bas au centre", u"Unten mittig", u"In basso al centro", u"Abajo al centro", u"Bawah tengah"}},
    {"BOTTOM RIGHT", {u"En bas à droite", u"Unten rechts", u"In basso a destra", u"Abajo a la derecha", u"Kanan bawah"}},
    {"FPS COUNTER", {u"Compteur d'images par seconde", u"FPS-Anzeige", u"Contatore FPS", u"Contador de FPS", u"Penghitung FPS"}},
    {"ANISOTROPIC FILTERING", {u"Filtrage anisotropique", u"Anisotropische Filterung", u"Filtro anisotropico", u"Filtro anisotrópico", u"Filter anisotropik"}},
    {"ANTI-ALIASING", {u"Antialiasing", u"Kantenglättung", u"Antialiasing", u"Suavizado de bordes", u"Anti-aliasing"}},
    {"LANGUAGE", {u"Langue", u"Sprache", u"Lingua", u"Idioma", u"Bahasa"}},
    {"AUDIO OUTPUT", {u"Sortie audio", u"Audioausgabe", u"Uscita audio", u"Salida de audio", u"Keluaran audio"}},
    {"CHEATS", {{}, {}, {}, {u"Trucos"}, u"Cheat"}},
    {"ANDROID OPTIONS", {{}, {}, {}, {u"Opción Android"}, u"Opsi Android"}},
    {"INFINITE NITROUS", {{}, {}, {}, {u"Nitro infinito"}, u"Nitro tak terbatas"}},
    {"INFINITE SPEEDBREAKER", {{}, {}, {}, {u"Rompevelocidad infinito"}, u"Speedbreaker tak terbatas"}},
    {"MAX CASH", {{}, {}, {}, {u"Dinero máximo"}, u"Uang maksimal"}},
    {"NO POLICE", {{}, {}, {}, {u"Sin policía"}, u"Tanpa polisi"}},
    {"UNLOCK ALL", {{}, {}, {}, {u"Desbloquear todo"}, u"Buka semua"}},
    {"NO TRAFFIC", {{}, {}, {}, {u"Sin tráfico"}, u"Tanpa lalu lintas"}},
    {"AUTO SHIFT IN DRAG", {{}, {}, {}, {u"Cambio automático en drag"}, u"Gigi otomatis di drag"}},
    {"SKIP MOVIES", {{}, {}, {}, {u"Saltar videos"}, u"Lewati video"}},
    {"CONTROLS OPACITY", {{}, {}, {}, {u"Opacidad de los controles"}, u"Transparansi kontrol"}},
    {"BUTTON DESIGN", {{}, {}, {}, {u"Diseño de los botones"}, u"Desain tombol"}},
    {"STICK MODE", {{}, {}, {}, {u"Modo de analógico"}, u"Mode analog"}},
    {"FLOATING", {{}, {}, {}, {u"Flotante"}, u"Mengambang"}},
    {"FIXED", {{}, {}, {}, {u"Fijo"}, u"Tetap"}},
    {"CONTROLS SIZE", {{}, {}, {}, {u"Tamaño de los controles"}, u"Ukuran kontrol"}},
    {"SPEEDOMETER", {u"Compteur de vitesse", u"Tachometer", u"Tachimetro", u"Velocímetro", u"Spidometer"}},
    {"MINIMAP", {u"Mini-carte", u"Minikarte", u"Minimappa", u"Minimapa", u"Peta mini"}},
    {"ZOOM IN", {u"Zoom avant", u"Vergrößern", u"Ingrandisci", u"Acercar", u"Perbesar"}},
    {"ZOOM OUT", {u"Zoom arrière", u"Verkleinern", u"Riduci", u"Alejar", u"Perkecil"}},
    {"RACE INFO", {u"Infos de course", u"Renninfo", u"Info gara", u"Info de carrera", u"Info balapan"}},
    {"EDIT CONTROLS POSITION", {{}, {}, {}, {u"Editar posición de los controles"}, u"Ubah posisi kontrol"}},
    {"STEERING", {{}, {}, {}, {u"Dirección"}, u"Kemudi"}},
    {"ARROWS", {{}, {}, {}, {u"Flechas"}, u"Panah"}},
    {"WHEEL", {{}, {}, {}, {u"Volante"}, u"Setir"}},
    {"TILT", {{}, {}, {}, {u"Inclinación"}, u"Miringkan HP"}},
    {"TILT ANGLE", {{}, {}, {}, {u"Ángulo de inclinación"}, u"Sudut kemiringan"}},
    {"CREDITS", {u"Crédits", u"Mitwirkende", u"Crediti", u"Créditos", u"Kredit"}},
    {"ANDROID PORT", {u"Portage Android", u"Android-Portierung", u"Porting Android", u"Port de Android", u"Port Android"}},
    {"Port by Rangga Casper", {u"Port par Rangga Casper", u"Portiert von Rangga Casper", u"Port di Rangga Casper",
                               u"Port de Rangga Casper", u"Port oleh Rangga Casper"}},
    {"RECOMPILER AND RUNTIME", {u"Recompilateur et runtime", u"Recompiler und Laufzeit", u"Ricompilatore e runtime", u"Recompilador y runtime", u"Recompiler dan runtime"}},
    {"LIBRARIES", {u"Bibliothèques", u"Bibliotheken", u"Librerie", u"Bibliotecas", u"Pustaka"}},
    {"XBOX REFERENCES", {u"Références Xbox", u"Xbox-Referenzen", u"Riferimenti Xbox", u"Referencias de Xbox", u"Referensi Xbox"}},
    {"SUPPORT", {u"Soutenir", u"Unterstützen", u"Supporta", u"Apoyar", u"Dukungan"}},
    {"DONATE", {u"Faire un don", u"Spenden", u"Dona", u"Donar", u"Donasi"}},

    // their values
    {"FULLSCREEN", {u"Plein écran", u"Vollbild", u"Schermo intero", u"Pantalla completa", u"Layar penuh"}},
    {"WINDOWED", {u"Fenêtré", u"Fenster", u"Finestra", u"Ventana", u"Jendela"}},
    {"BORDERLESS", {u"Fenêtré sans bordure", u"Randloses Fenster", u"Finestra senza bordi", u"Ventana sin bordes", u"Jendela tanpa bingkai"}},
    {"ON", {u"Oui", u"Ein", u"Sì", u"Sí", u"Aktif"}},
    {"OFF", {u"Non", u"Aus", u"No", u"No", u"Mati"}},
    {"UNLIMITED", {u"Illimité", u"Unbegrenzt", u"Illimitato", u"Ilimitado", u"Tanpa batas"}},
    {"REFRESH RATE", {u"Fréquence de rafraîchissement", u"Bildwiederholfrequenz", u"Frequenza di aggiornamento", u"Frecuencia de actualización", u"Refresh rate layar"}},
    {"AUTOMATIC", {u"Automatique", u"Automatisch", u"Automatico", u"Automático", u"Otomatis"}},
    {"GAME DEFAULT", {u"Par défaut du jeu", u"Spielstandard", u"Predefinito del gioco", u"Predeterminado del juego", u"Bawaan game"}},
    {"NOT SUPPORTED", {u"Non pris en charge", u"Nicht unterstützt", u"Non supportato", u"No compatible", u"Tidak didukung"}},
    {"MONO", {u"Mono", u"Mono", u"Mono", u"Mono", u"Mono"}},
    {"2.0 STEREO", {u"2.0 Stéréo", u"2.0 Stereo", u"2.0 Stereo", u"2.0 Estéreo", u"2.0 Stereo"}},
    {"5.1 SURROUND", {u"5.1 Surround", u"5.1 Surround", u"5.1 Surround", u"5.1 Surround", u"5.1 Surround"}},
    {"(DEFAULT)", {u"(par défaut)", u"(Standard)", u"(predefinito)", u"(predeterminado)", u"(bawaan)"}},
    {"(RESTART)", {u"(redémarrage)", u"(Neustart)", u"(riavvio)", u"(reiniciar)", u"(mulai ulang)"}},
    {"ENGLISH", {u"Anglais", u"Englisch", u"Inglese", u"Inglés", u"Inggris"}},
    {"FRENCH", {u"Français", u"Französisch", u"Francese", u"Francés", u"Prancis"}},
    {"GERMAN", {u"Allemand", u"Deutsch", u"Tedesco", u"Alemán", u"Jerman"}},
    {"ITALIAN", {u"Italien", u"Italienisch", u"Italiano", u"Italiano", u"Italia"}},
    {"SPANISH", {u"Espagnol", u"Spanisch", u"Spagnolo", u"Español", u"Spanyol"}},
    // its own name in every language, so it can be found from any of them
    {"INDONESIAN", {u"Bahasa Indonesia", u"Bahasa Indonesia", u"Bahasa Indonesia", u"Bahasa Indonesia", u"Bahasa Indonesia"}},
    {"OPEN", {{}, {}, {}, {u"Abrir"}, u"Buka"}},
    {"CHECK FOR UPDATES", {u"Rechercher des mises à jour", u"Nach Updates suchen", u"Cerca aggiornamenti", u"Buscar actualizaciones", u"Cek pembaruan"}},
    {"CHECK", {u"Rechercher", u"Suchen", u"Cerca", u"Buscar", u"Cek"}},
    {"DEFAULT", {{}, {}, {}, {u"Predeterminado"}, u"Bawaan"}},

    {"SETTINGS PANEL", {u"Panneau des paramètres", u"Einstellungsmenü", u"Pannello impostazioni", u"Panel de configuración", u"Panel pengaturan"}},

    // the Xbox pad's inputs, the KEYBOARD and CONTROLLER pages' rows
    {"BLACK", {{}, {}, {}, {u"Botón negro"}, u"Tombol hitam"}},
    {"WHITE", {{}, {}, {}, {u"Botón blanco"}, u"Tombol putih"}},
    {"LEFT TRIGGER", {{}, {}, {}, {u"Gatillo izquierdo"}, u"Trigger kiri"}},
    {"RIGHT TRIGGER", {{}, {}, {}, {u"Gatillo derecho"}, u"Trigger kanan"}},
    {"DPAD UP", {{}, {}, {}, {u"Cruceta arriba"}, u"D-pad atas"}},
    {"DPAD DOWN", {{}, {}, {}, {u"Cruceta abajo"}, u"D-pad bawah"}},
    {"DPAD LEFT", {{}, {}, {}, {u"Cruceta a la izq."}, u"D-pad kiri"}},
    {"DPAD RIGHT", {{}, {}, {}, {u"Cruceta a la der."}, u"D-pad kanan"}},
    {"LEFT THUMB", {{}, {}, {}, {u"Presionar analógico izq."}, u"Tekan analog kiri"}},
    {"RIGHT THUMB", {{}, {}, {}, {u"Presionar analógico der."}, u"Tekan analog kanan"}},
    {"LEFT STICK UP", {{}, {}, {}, {u"Analógico izq. arriba"}, u"Analog kiri atas"}},
    {"LEFT STICK DOWN", {{}, {}, {}, {u"Analógico izq. abajo"}, u"Analog kiri bawah"}},
    {"LEFT STICK LEFT", {{}, {}, {}, {u"Analógico izq. a la izq."}, u"Analog kiri ke kiri"}},
    {"LEFT STICK RIGHT", {{}, {}, {}, {u"Analógico izq. a la der."}, u"Analog kiri ke kanan"}},
    {"RIGHT STICK UP", {{}, {}, {}, {u"Analógico der. arriba"}, u"Analog kanan atas"}},
    {"RIGHT STICK DOWN", {{}, {}, {}, {u"Analógico der. abajo"}, u"Analog kanan bawah"}},
    {"RIGHT STICK LEFT", {{}, {}, {}, {u"Analógico der. a la izq."}, u"Analog kanan ke kiri"}},
    {"RIGHT STICK RIGHT", {{}, {}, {}, {u"Analógico der. a la der."}, u"Analog kanan ke kanan"}},

    // a hint's "PRESS (A) :" with the key in place of the button
    {"PRESS", {u"Appuyez sur", u"Drücke", u"Premi", u"Presiona", u"Tekan"}},

    // the face buttons' captions, as the loading screen names the controls (native/touch.cpp's kButtons)
    {"E-BRAKE", {u"Frein à main", u"Handbremse", u"Freno a mano", u"Freno de mano", u"Rem tangan"}},
    {"NITROUS", {u"Nitro", u"Nitro", u"Nitro", u"Nitro", u"Nitro"}},
    {"SPEEDBREAKER", {u"Antidérapant", u"Speedbreaker", u"Speedbreaker", u"Rompevelocidad", u"Speedbreaker"}},
    // the pedals' names
    {"GAS", {u"Accélérateur", u"Gas", u"Acceleratore", u"Gas", u"Gas"}},
    {"BRAKE", {u"Frein", u"Bremse", u"Freno", u"Freno", u"Rem"}},
    {"LOOK BACK", {u"Regarder derrière", u"Rückblick", u"Guarda dietro", u"Ver atrás", u"Lihat belakang"}},
    {"CAMERA", {u"Caméra", u"Kamera", u"Camera", u"Cámara", u"Kamera"}},

    // the F10 panel's own
    {"SETTINGS", {u"Paramètres", u"Einstellungen", u"Impostazioni", u"Configuración", u"Pengaturan"}},
    {"BACK", {u"Retour", u"Zurück", u"Indietro", u"Atrás", u"Kembali"}},
    {"DEFAULTS", {u"Par défaut", u"Standard", u"Predefiniti", u"Por defecto", u"Bawaan"}},
    {"MOUSE", {u"Souris", u"Maus", u"Mouse", u"Ratón", u"Mouse"}},
    {"KEYBOARD", {u"Clavier", u"Tastatur", u"Tastiera", u"Teclado", u"Keyboard"}},
    {"Press a key or mouse button to bind, Escape cancels", {u"Appuyez sur une touche ou un bouton de souris, Échap pour annuler", u"Taste oder Maustaste drücken, Escape bricht ab", u"Premi un tasto o un pulsante del mouse, Esc per annullare", u"Presiona una tecla o un botón del ratón, Escape para cancelar", u"Tekan tombol keyboard atau mouse untuk mengatur, Escape untuk batal"}},
    {"Right click or Delete clears the key", {u"Clic droit ou Suppr pour effacer la touche", u"Rechtsklick oder Entf löscht die Taste", u"Clic destro o Canc per cancellare il tasto", u"Clic derecho o Supr para borrar la tecla", u"Klik kanan atau Delete untuk menghapus tombol"}},
    {"CONTROLLER", {u"Manette", u"Controller", u"Controller", u"Control", u"Controller"}},
    {"PRESS A BUTTON", {u"Appuyez sur un bouton", u"Taste drücken", u"Premi un pulsante", u"Presiona un botón", u"Tekan tombol"}},
    {"Press a button on the controller to bind, wait 5 seconds to cancel", {u"Appuyez sur un bouton de la manette, attendez 5 secondes pour annuler", u"Controller-Taste drücken, 5 Sekunden warten bricht ab", u"Premi un pulsante del controller, attendi 5 secondi per annullare", u"Presiona un botón del control, espera 5 segundos para cancelar", u"Tekan tombol di controller untuk mengatur, tunggu 5 detik untuk batal"}},
    {"Right click or Delete restores the default", {u"Clic droit ou Suppr pour rétablir la valeur par défaut", u"Rechtsklick oder Entf stellt den Standard wieder her", u"Clic destro o Canc per ripristinare il predefinito", u"Clic derecho o Supr para restaurar el predeterminado", u"Klik kanan atau Delete untuk kembali ke bawaan"}},

    // the touch layout editor
    {"RESET", {{}, {}, {}, {u"Restablecer"}, u"Atur ulang"}},
    {"CANCEL", {{}, {}, {}, {u"Cancelar"}, u"Batal"}},
    {"HIDE", {{}, {}, {}, {u"Ocultar"}, u"Sembunyikan"}},
    {"SHOW", {{}, {}, {}, {u"Mostrar"}, u"Tampilkan"}},
    {"DONE", {{}, {}, {}, {u"Listo"}, u"Selesai"}},
    {"MENUS", {{}, {}, {}, {u"Menús"}, u"Menu"}},
    {"RACE", {{}, {}, {}, {u"Carrera"}, u"Balapan"}},
    {"MENU CONTROLS: DRAG TO MOVE, PINCH OR - + TO RESIZE", {{}, {}, {}, {u"Controles del menú: arrastra para mover, pellizca o - + para cambiar el tamaño"},
                                                             u"Kontrol menu: seret untuk memindah, cubit atau - + untuk mengubah ukuran"}},

    // a menu's A and B pills over the touch controls
    {"ENTER", {{}, {}, {}, {u"Elegir"}, u"Pilih"}},
    {"CLOSE", {{}, {}, {}, {u"Cerrar"}, u"Tutup"}},
    {"PREV", {{}, {}, {}, {u"Anterior"}, u"Mundur"}},
    {"NEXT", {{}, {}, {}, {u"Siguiente"}, u"Maju"}},

    // the message boxes before the game runs (native/). Whole sentences need every language: one with no text in
    // a language would have its words swapped one by one ("GAME", "ON", ...)
    {"is a different release of the game than this build was made from, and will most likely crash.\n\nStart it "
     "anyway?",
     {u"est une autre version du jeu que celle de cette build, et plantera très probablement.\n\nLe lancer quand "
      u"même ?",
      u"ist eine andere Version des Spiels als die, aus der dieser Build gemacht wurde, und stürzt sehr "
      u"wahrscheinlich ab.\n\nTrotzdem starten?",
      u"è una versione del gioco diversa da quella di questa build, e molto probabilmente andrà in crash.\n\n"
      u"Avviarlo comunque?",
      u"es otra versión del juego, distinta de la que se usó para hacer esta build, y casi seguro se va a cerrar.\n\n"
      u"¿Iniciarla de todos modos?",
      u"adalah rilis game yang berbeda dari yang dipakai build ini, dan kemungkinan besar akan crash.\n\nTetap "
      u"jalankan?"}},
    {"YES", {u"Oui", u"Ja", u"Sì", u"Sí", u"Ya"}},
    {"NO", {u"Non", u"Nein", u"No", u"No", u"Tidak"}},
    {"The game failed to start, see the log in the logs folder.",
     {u"Le jeu n'a pas pu démarrer, voir le journal dans le dossier logs.",
      u"Das Spiel konnte nicht starten, siehe das Protokoll im Ordner logs.",
      u"Il gioco non è riuscito ad avviarsi, vedi il log nella cartella logs.",
      u"El juego no pudo iniciarse, consulta el registro en la carpeta logs.",
      u"Game gagal dimulai, lihat log di folder logs."}},
};

bool Same(std::string_view a, std::string_view b) {
    auto upper = [](char c) { return char(std::toupper(uint8_t(c))); };
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [&](char x, char y) {
               return upper(x) == upper(y);
           });
}

bool WordChar(char c) { return std::isalnum(uint8_t(c)) != 0; }

// the language the game booted in, -1 for English or one this disc cannot boot in
int Language() {
    static const int language = [] {
        if (!game::LanguageAvailable(game::BootLanguage())) return -1;
        auto at = std::find(std::begin(game::kLanguages), std::end(game::kLanguages), game::BootLanguage());
        return at == std::end(game::kLanguages) ? -1 : int(at - std::begin(game::kLanguages)) - 1;
    }();
    return language;
}

// null where the entry has none in this language, so the English shows
const char16_t* Translation(std::string_view english) {
    for (const Entry& e : kEntries)
        if (Same(e.english, english)) return e.text[Language()].empty() ? nullptr : e.text[Language()].data();
    return nullptr;
}

// the longest phrase with a translation from text[at], at a word start; end set past it
const char16_t* PhraseAt(std::string_view text, size_t at, size_t& end) {
    if (at && WordChar(text[at - 1])) return nullptr;
    for (end = text.size(); end > at; end--) {
        if (end < text.size() && WordChar(text[end])) continue;
        if (const char16_t* found = Translation(text.substr(at, end - at))) return found;
    }
    return nullptr;
}

// latin 1 lower case to upper; 0xF7 is the division sign
char16_t Upper(char16_t c) {
    bool lower = (c >= u'a' && c <= u'z') || (c >= 0xE0 && c <= 0xFE && c != 0xF7);
    return lower ? char16_t(c - 0x20) : c;
}

void AppendUtf8(std::string& out, char16_t c) {
    if (c < 0x80) {
        out += char(c);
    } else if (c < 0x800) {
        out += char(0xC0 | c >> 6);
        out += char(0x80 | (c & 0x3F));
    } else {
        out += char(0xE0 | c >> 12);
        out += char(0x80 | ((c >> 6) & 0x3F));
        out += char(0x80 | (c & 0x3F));
    }
}

}  // namespace

std::string app::Ui(std::string_view text, bool caps) {
    if (Language() < 0) return std::string(text);
    // the panel's thread and the game's
    static std::mutex lock;
    static std::map<std::string, std::string> cache;
    std::lock_guard guard(lock);
    std::string key = std::string(text) + (caps ? "\1" : "\2");
    if (auto it = cache.find(key); it != cache.end()) return it->second;
    std::string out;
    size_t i = 0;
    while (i < text.size()) {
        size_t end = 0;
        const char16_t* found = PhraseAt(text, i, end);
        if (!found) {
            out += text[i++];
            continue;
        }
        for (; *found; found++) AppendUtf8(out, caps ? Upper(*found) : *found);
        i = end;
    }
    return cache[key] = out;
}
