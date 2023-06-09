# MicroLab2
For the 3rd lab


Κώδικας:
 Στην main() ορίζουμε τα αρχικά setups. Με την χρήση της uart_print ζητάμε το ΑΕΜ του χρήστη που θα καθορίσει το ρυθμό δειγματοληψίας μετέπειτα. Με μια while με το flag : scan_has_ended ουσιαστικά "παγώνουμε" την main μέχρι ο χρήστης να δώσει ΑΕΜ. Μόλις συμβεί αυτό ο timer ξεκινάει να μετράει. Κάθε ένα δευτερόλεπτο καλείται έτσι interrupt. Ανάλογα με την περίοδο που έχει τεθεί από το ΑΕΜ, σε interrupt του timer καλείται η ρουτίνα που είναι υπεύθυνη για την επικοινωνία με τον αισθητήρα.
 
 Η επικοινωνία με τον αισθητήρα γίνεται σε δύο στάδια, πρώτα ο επεξεργαστής στέλνει το σήμα επικοινωνίας με τον αισθητήρα και έπειτα , αν πάρει απάντηση, ο αισθητήρας στέλνει ένα-ένα τα 40-bit σύμφωνα με το documentation. Στο τέλος, γίνεται μετατροπή των bit σε πληροφορία θερμοκρασίας(ακέραιο και δεκαδικό μέρος) καθώς και τυπώνεται στην οθόνη. Υπάρχει παράλληλα και το interrupt του κουμπιού, το οποίο αν κληθεί μετράει πόσες φορές έχει πατηθεί και αλλάζει την περίοδο ανάλογα με το αν ο αριθμός αυτός είναι μονός ή ζυγός.
 
  Πιο ειδικά για την επικοινωνία με τον αισθητήρα:
  
  Στο πρώτο στάδιο (first_call()) γίνεται η επικοινωνία του επεξεργαστή με τον αισθητήρα μέσω του PIN που έχουμε ορίσει. Θέτουμε δηλαδή το PIN ως output και για συγκεκριμένο χρόνο, με τη χρήση της delay_ms(), θέτουμε το σήμα στο low. Μια φιλοσοφία κώδικα που χρησιμοποιήσαμε και εδώ και σε άλλα μέρη, είναι η while(gpio_get(PINAKI) == 0) χωρίς περιεχόμενο, ώστε το πρόγραμμα να "παγώνει" σε εκείνο το σημείο, μέχρι να πάψει να αληθεύει η συνθήκη.
  Στο δεύτερο στάδιο δεχόμαστε τις τιμές μια μια και με ένα shift left κρατούμε τις τιμές των bit στην μεταβλητή value. Σύμφωνα με το documentation ο αισθητήρας για να μεταδώσει το λογικό 1 θα κρατήσει για 70 μs την τιμή σε high, ενώ για το λογικό 0 για 28 μs. Κάθε φορά πριν σταλεί ένα bit θα θέσει το πιν σε low για 50 μs.
  Έτσι για απλότητα ελέγχουμε με μια while(gpio_get(PINAKI)==0); πότε θα τελειώσουν τα 50 μs. Έπειτα από ένα delay_us(35) ελέγχουμε ξανά την τιμή που έχει το πιν.
  Αν η τιμή είναι high σημαίνει πως το bit είναι 1, αλλιώς 0. Έπειτα μετράμε 60μs για να είμαστε σίγουροι πως δεν θα πέσουμε σε κάποια αλλαγή κατάστασης. Τέλος, με κατάλληλες μάσκες και shift παίρνουμε τις τιμές που επιστρέφει ο αισθητήρας και ελέγχουμε το άθροισμα (checksum) ώστε να βεβαιωθούμε πως δεν έχει γίνει κάποιο λάθος στην μετάδοση.
  
  
  Προβλήματα:
  
  H πρώτη πρώτη τιμή που λαμβάνουμε από τον αισθητήρα όταν ξεκινάει το πρόγραμμα συνήθως είναι λανθασμένη, οπότε την αγνοούμε.
  
  Testing: 
  Έγινε το απαραίτητο testing σύμφωνα με το εργαστήριο, καθώς και αλλάξαμε τις τιμές της θερμοκρασίας για να ελέγξουμε και αν αναβόσβηνε το λεντάκι σωστά.

