<?php
header('Content-Type: application/json');

require_once __DIR__ . '/../includes/db.php';

try {
    // Delete all active sessions
    $pdo->exec("DELETE FROM active_session");
    echo json_encode(['success' => true]);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>
