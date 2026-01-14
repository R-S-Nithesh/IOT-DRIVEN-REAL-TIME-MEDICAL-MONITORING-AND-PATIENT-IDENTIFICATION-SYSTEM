<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

$id = $_POST['id'] ?? $_GET['id'] ?? 0;

if (!$id) {
    echo json_encode(['success' => false, 'error' => 'Patient ID required']);
    exit;
}

try {
    // Delete patient (CASCADE will delete readings and sessions)
    $stmt = $pdo->prepare("DELETE FROM patients WHERE id = ?");
    $stmt->execute([$id]);

    echo json_encode(['success' => true]);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>
