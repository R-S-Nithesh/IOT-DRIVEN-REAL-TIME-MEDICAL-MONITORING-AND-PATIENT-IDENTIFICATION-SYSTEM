<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

try {
    $stmt = $pdo->query("SELECT * FROM patients ORDER BY created_at DESC");
    $patients = $stmt->fetchAll();
    echo json_encode(['success' => true, 'patients' => $patients]);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>
